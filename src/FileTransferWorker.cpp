#include "FileTransferWorker.h"

FileTransferException::FileTransferException(QString const& message)
{
    this->message = message;
}

void FileTransferException::raise() const
{
    throw *this;
}

FileTransferException* FileTransferException::clone() const
{
    return new FileTransferException(*this);
};

QString FileTransferException::getMessage() const
{
    return this->message;
}

FileTransferWorker::FileTransferWorker(std::shared_ptr<FileTransferJob> job)
{
    this->job = job;
    this->fileOverwriteAnswer = FileOverwriteAnswer::None;
}

FileTransferWorker::~FileTransferWorker()
{

}

void FileTransferWorker::process()
{
    this->job->setState(FileTransferState::Connecting);

    auto connEntry = this->job->getConnEntry();
    this->conn = SSHConnectionManager::getInstance().createSSHConnection(connEntry);

    if (this->conn == nullptr) {
        this->job->setState(FileTransferState::Failed);
        emit finished();
        return;
    }

    if (conn->sftp == nullptr) {
        while ((conn->sftp = libssh2_sftp_init(conn->session)) == nullptr &&
                libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN)  {
            SSHConnectionManager::waitsocket(conn);
        }

        if (conn->sftp == nullptr) {
            QString error = QString("Unable to init SFTP session: ")
                + QString::number(libssh2_session_last_error(conn->session,NULL,NULL,0));
            std::cerr << error.toStdString() << "\n";
            this->job->setErrorMessage(error);
            this->job->setState(FileTransferState::FailedConnect);
            emit finished();
            return;
        }
    }

    if (conn == nullptr) {
        this->job->setState(FileTransferState::Failed);
        emit finished();
        return;
    }

    this->job->setState(FileTransferState::Running);

    bool success = true;
    try {
        for (QString const &filename : job->getFilesToCopy()) {
            if (job->getType() == FileTransferType::Download) {
                this->copyFileFromRemoteRecursively(filename, job->getTargetDir());
            } else {
                this->copyFileToRemoteRecursively(filename, job->getTargetDir());
            }
        }
    } catch (const FileTransferException e) {
        this->job->setErrorMessage(e.getMessage());
        this->job->setState(FileTransferState::Failed);
        success = false;
    }

    if (success) {
        this->job->setState(FileTransferState::Completed);
    }
    emit finished();
}

void FileTransferWorker::copyFileFromRemoteRecursively(QString remotePath, QString localDir)
{
    int rc;
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    if (this->conn == nullptr) {
        return;
    }

    while ((rc = libssh2_sftp_realpath(this->conn->sftp,
                    remotePath.toLatin1().data(), this->buffer, sizeof(this->buffer))) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc <= 0) {
        QString error = QString("Failed to resolve realpath for: ") + remotePath;
        std::cerr << error.toStdString() << "\n";
        throw FileTransferException(error);
    }

    std::cerr << "realpath: " << this->buffer << "\n";

    while ((rc = libssh2_sftp_stat(this->conn->sftp, this->buffer, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc < 0) {
        QString error = "Failed stat over SFTP: " + remotePath;
        std::cerr << error.toStdString() << "\n";
        throw FileTransferException(error);
    }

    if (LIBSSH2_SFTP_S_ISREG(attrs.permissions)) {
        std::cerr << "Is regular file: " << remotePath.toStdString() << "\n";
        this->copyFileFromRemote(remotePath, localDir);
    } else if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions)) {
        std::cerr << "Is directory: " << remotePath.toStdString() << "\n";
        QString basename = Util::basename(remotePath);

        QDir dir(localDir);
        dir.mkdir(basename);
        localDir += "/" + basename;

        auto entries = SSHConnectionManager::getInstance().doReadDirectory(this->conn, remotePath, false);
        for (auto const &entry : entries) {
            QString newBasename = "";
            QString newRemotePath = remotePath + "/" + entry->getFilename();

            if (entry->isDirectory() || entry->isSymLink()) {
                this->copyFileFromRemoteRecursively(newRemotePath, localDir);
            } else if (entry->isRegularFile()) {
                this->copyFileFromRemote(newRemotePath, localDir);
            }
        }
    } else {
        // Ignore
        std::cerr << "Ignoring file '" << remotePath.toStdString() << "' of unknown type" << "\n";
    }
}

void FileTransferWorker::copyFileToRemoteRecursively(QString localPath, QString remoteDir)
{
    if (this->conn == nullptr) {
        throw FileTransferException("No connection");
    }

    std::cerr << "Copy file recursively: " << localPath.toStdString() << " to " << remoteDir.toStdString() << "\n";
    QFileInfo fileInfo(localPath);

    if (fileInfo.isDir()) {
        QString basename = Util::basename(localPath);
        QString newRemoteDir = remoteDir + "/" + basename;

        while (libssh2_sftp_mkdir(this->conn->sftp, newRemoteDir.toLatin1().data(),
                    LIBSSH2_SFTP_S_IRWXU|
                    LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IXGRP|
                    LIBSSH2_SFTP_S_IROTH|LIBSSH2_SFTP_S_IXOTH)
                == LIBSSH2_ERROR_EAGAIN);

        QDir dir(localPath);
        QStringList fileList = dir.entryList();
        for (auto const &filename : fileList) {
            if (filename == "." || filename == "..") {
                continue;
            }

            QString newLocalPath = localPath + "/" + filename;
            QFileInfo curFileInfo(newLocalPath);

            if (curFileInfo.isDir()) {
                this->copyFileToRemoteRecursively(newLocalPath, newRemoteDir);
            } else if (curFileInfo.isFile()) {
                this->copyFileToRemote(newLocalPath, newRemoteDir);
            }
        }
    } else if (fileInfo.isFile()) {
        this->copyFileToRemote(localPath, remoteDir);
    }
}

void FileTransferWorker::copyFileFromRemote(QString remotePath, QString localDir)
{
    int rc = 0;
    int nread = 0;
    char *ptr = NULL;
    FILE *fp = NULL;
    char buffer[1024 * 4];
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    uint64_t total = 0;
    QString errorToThrow;
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    std::cerr << "Downloading file " << remotePath.toStdString() << "\n";

    if (this->conn == nullptr) {
        std::cerr << "No connection\n";
        throw FileTransferException("No connection");
    }

    QString localFilename = localDir + "/" + Util::basename(remotePath);

    do {
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remotePath.toLatin1().data(),
                LIBSSH2_FXF_READ, 0);

        if (!sftp_handle && (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {
            QString error = "Unable to open file with SFTP: " + remotePath;
            std::cerr << error.toStdString() << "\n";
            throw FileTransferException(error);
        }
    } while (!sftp_handle);

    if (this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
        QFileInfo fileInfo(localFilename);

        if (fileInfo.exists()) {
            if (this->fileOverwriteAnswer == FileOverwriteAnswer::NoToAll) {
                goto shutdown;
            }

            this->waitUntilFileOverwriteAnswerChanged("Download from "
                    + this->job->getConnEntry()->getIdentifier(),
                    "Local file '" + localFilename + "' already exists!",
                    "Do you want to overwrite this file?");

            if (this->fileOverwriteAnswer != FileOverwriteAnswer::Yes
                    && this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
                goto shutdown;
            }
        }
    }

    fp = fopen(localFilename.toLatin1().data(), "w");
    if (!fp) {
        errorToThrow = "Can't open local file: " + localFilename;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

    do {
        while ((nread = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer))) ==
                LIBSSH2_ERROR_EAGAIN) {
            SSHConnectionManager::waitsocket(this->conn);
        }

        if (nread <= 0) {
            /* end of file */
            break;
        }

        ptr = buffer;
        total += nread;

        do {
            rc = fwrite(ptr, 1, nread, fp);
            ptr += rc;
            nread -= rc;
        } while (nread);
    } while (rc > 0);

    fclose(fp);

    // set file permissions
    while ((rc = libssh2_sftp_fstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc != 0) {
        errorToThrow = "Failed stat over SFTP: " + remotePath;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

    if (chmod(localFilename.toLatin1().data(), attrs.permissions) != 0) {
        errorToThrow = "Failed chmod for file: " + localFilename;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

    std::cerr << "Finished downloading file " << remotePath.toStdString() << "\n";

shutdown:
    libssh2_sftp_close(sftp_handle);

    if (!errorToThrow.isEmpty()) {
        throw FileTransferException(errorToThrow);
    }
}

void FileTransferWorker::copyFileToRemote(QString localPath, QString remoteDir)
{
    int rc = 0;
    int nread = 0;
    char *ptr = NULL;
    FILE *fp = NULL;
    char buffer[1024*4];
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    uint64_t total = 0;
    QString errorToThrow;

    if (this->conn == nullptr) {
        throw FileTransferException("No connection");
    }

    std::cerr << "Uploading file " << localPath.toStdString() << "\n";

    fp = fopen(localPath.toLatin1().data(), "rb");
    if (!fp) {
        QString error = "Can't open local file: " + localPath;
        std::cerr << error.toStdString() << "\n";
        throw FileTransferException(error);
    }

    QString remoteFilename = remoteDir + "/" + Util::basename(localPath);

    do {
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remoteFilename.toLatin1().data(),
                LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

        if (!sftp_handle &&
                (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {

            QString error = "Unable to open file '" + remoteFilename + "' with SFTP";
            std::cerr << error.toStdString() << "\n";
            throw FileTransferException(error);
        }
    } while (!sftp_handle);

    do {
        nread = fread(buffer, 1, sizeof(buffer), fp);
        if (nread <= 0) {
            /* end of file */
            break;
        }

        ptr = buffer;
        total += nread;

        do {
            /* write data in a loop until we block */
            while ((rc = libssh2_sftp_write(sftp_handle, ptr, nread)) ==
                    LIBSSH2_ERROR_EAGAIN) {
                SSHConnectionManager::waitsocket(this->conn);
            }
            if(rc < 0)
                break;
            ptr += rc;
            nread -= rc;
        } while (nread);
    } while (rc > 0);

    // set file permissions
    struct stat statbuf;
    QFile file(localPath);
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    if (stat(localPath.toLatin1().data(), &statbuf) != 0) {
        errorToThrow = "Failed stat for file: " + localPath;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

    while ((rc = libssh2_sftp_fstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc != 0) {
        errorToThrow = "Failed stat over SFTP: " + remoteFilename;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

    attrs.permissions = statbuf.st_mode;

    while ((libssh2_sftp_fsetstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc != 0) {
        errorToThrow = "Failed change permissions over SFTP: " + remoteFilename;
        std::cerr << errorToThrow.toStdString() << "\n";
        goto shutdown;
    }

shutdown:
    libssh2_sftp_close(sftp_handle);

    if (!errorToThrow.isEmpty()) {
        throw FileTransferException(errorToThrow);
    }

    std::cerr << "Finished uploading file " << localPath.toStdString() << "\n";
}

void FileTransferWorker::setFileOverwriteAnswer(FileOverwriteAnswer answer)
{
    this->fileOverwriteAnswer = answer;
}

void FileTransferWorker::setFileOverwriteAnswerAndNotify(FileOverwriteAnswer answer)
{
    this->mutex.lock();

    this->setFileOverwriteAnswer(answer);
    this->fileOverwriteAnswerConditionVar.wakeOne();

    this->mutex.unlock();
}

FileOverwriteAnswer FileTransferWorker::getFileOverwriteAnswer()
{
    return this->fileOverwriteAnswer;
}

void FileTransferWorker::waitUntilFileOverwriteAnswerChanged(QString title, QString message, QString infoText)
{
    this->mutex.lock();

    this->fileOverwriteAnswer = FileOverwriteAnswer::None;
    emit askToOverwriteFile(title, message, infoText);

    while (this->fileOverwriteAnswer == FileOverwriteAnswer::None) {
        this->fileOverwriteAnswerConditionVar.wait(&this->mutex);
    }

    this->mutex.unlock();
}
