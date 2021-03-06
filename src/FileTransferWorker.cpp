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
}

QString FileTransferException::getMessage() const
{
    return this->message;
}

FileTransferWorker::FileTransferWorker(std::shared_ptr<FileTransferJob> job)
{
    this->job = job;
    this->fileOverwriteAnswer = FileOverwriteAnswer::None;
    this->lastTransferTime = 0;
    this->accumulatedByteDiff = 0;
    this->conn = nullptr;
}

FileTransferWorker::~FileTransferWorker()
{

}

void FileTransferWorker::process()
{
    this->job->setState(FileTransferState::Connecting);

    auto connEntry = this->job->getConnEntry();

    // Create a new connection for the download. When we run inside a test
    // this->conn is already set by the calling test code.
    if (this->conn == nullptr) {
        this->conn = SSHConnectionManager::getInstance().createSSHConnection(connEntry);
    }

    if (this->conn == nullptr) {
        this->job->setState(FileTransferState::Failed);

        emit finished();
        return;
    }

    if (conn->sftp == nullptr) {
        while ((conn->sftp = libssh2_sftp_init(conn->session)) == nullptr &&
                libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN) {
            CHECK_CANCEL();
            QThread::msleep(this->sleeptime);
            CHECK_CANCEL();
        }

        if (conn->sftp == nullptr) {
            QString error = QString("Unable to init SFTP session: ")
                + QString::number(libssh2_session_last_error(conn->session,NULL,NULL,0));
            qDebug() << error;
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

    this->transferTimer.start();
    this->lastTransferTime = this->transferTimer.elapsed();
    this->job->setState(FileTransferState::InProgress);

    try {
        for (QString const &filename : job->getFilesToCopy()) {
            CHECK_CANCEL();
            if (job->getType() == FileTransferType::Download) {
                this->copyFileFromRemoteRecursively(filename, job->getTargetDir());
            } else {
                this->copyFileToRemoteRecursively(filename, job->getTargetDir());
            }
            CHECK_CANCEL();
        }

        this->job->setState(FileTransferState::Completed);
    } catch (const FileTransferException e) {
        this->job->setErrorMessage(e.getMessage());
        this->job->setState(FileTransferState::Failed);
    } catch (const FileTransferCancelException) {
        this->job->setState(FileTransferState::Canceled);
    }

    emit finished();
}

void FileTransferWorker::copyFileFromRemoteRecursively(QString remotePath, QString localDir)
{
    int rc;
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    if (this->conn == nullptr) {
        this->job->setState(FileTransferState::FailedConnect);
        return;
    }

    while ((rc = libssh2_sftp_realpath(this->conn->sftp,
                    remotePath.toLatin1().data(), this->realPathBuffer, sizeof(this->realPathBuffer))) == LIBSSH2_ERROR_EAGAIN) {
        CHECK_CANCEL();
        QThread::msleep(this->sleeptime);
        CHECK_CANCEL();
    }

    if (rc <= 0) {
        QString error = QString("Failed to resolve realpath for: ") + remotePath;
        qDebug() << error;
        throw FileTransferException(error);
    }

    while ((rc = libssh2_sftp_stat(this->conn->sftp, this->realPathBuffer, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        CHECK_CANCEL();
        QThread::msleep(this->sleeptime);
        CHECK_CANCEL();
    }

    if (rc < 0) {
        QString error = "Failed stat over SFTP: " + remotePath;
        qDebug() << error;
        throw FileTransferException(error);
    }

    if (LIBSSH2_SFTP_S_ISREG(attrs.permissions)) {
        qDebug() << "Is regular file: " << remotePath;
        this->copyFileFromRemote(remotePath, localDir);
    } else if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions)) {
        qDebug() << "Is directory: " << remotePath;
        QString basename = Util::basename(remotePath);

        QDir dir(localDir);
        dir.mkdir(basename);
        localDir += "/" + basename;

        auto entries = SSHConnectionManager::getInstance().doReadDirectory(this->conn, remotePath, false);
        for (auto const &entry : entries) {
            QString newBasename = "";
            QString newRemotePath = remotePath + "/" + entry->getFilename();

            if (entry->isDirectory() || entry->isSymLink()) {
                CHECK_CANCEL();
                this->copyFileFromRemoteRecursively(newRemotePath, localDir);
                CHECK_CANCEL();
            } else if (entry->isRegularFile()) {
                CHECK_CANCEL();
                this->copyFileFromRemote(newRemotePath, localDir);
                CHECK_CANCEL();
            }
        }
    } else {
        // Ignore
        qDebug() << "Ignoring file '" << remotePath << "' of unknown type";
    }
}

void FileTransferWorker::copyFileToRemoteRecursively(QString localPath, QString remoteDir)
{
    if (this->conn == nullptr) {
        throw FileTransferException("No connection");
    }

    qDebug() << "Copy file recursively: " << localPath << " to " << remoteDir;
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
                CHECK_CANCEL();
                this->copyFileToRemoteRecursively(newLocalPath, newRemoteDir);
                CHECK_CANCEL();
            } else if (curFileInfo.isFile()) {
                CHECK_CANCEL();
                this->copyFileToRemote(newLocalPath, newRemoteDir);
                CHECK_CANCEL();
            }
        }
    } else if (fileInfo.isFile()) {
        CHECK_CANCEL();
        this->copyFileToRemote(localPath, remoteDir);
        CHECK_CANCEL();
    }
}

void FileTransferWorker::copyFileFromRemote(QString remotePath, QString localDir)
{
    int rc = 0;
    int nread = 0;
    char *ptr = NULL;
    FILE *fp = NULL;
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    QString errorString;
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    qDebug() << "Downloading file " << remotePath;

    if (this->conn == nullptr) {
        qDebug() << "No connection";
        throw FileTransferException("No connection");
    }

    QString localFilename = localDir + "/" + Util::basename(remotePath);

    do {
        CHECK_CANCEL();
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remotePath.toLatin1().data(),
                LIBSSH2_FXF_READ, 0);

        if (!sftp_handle && (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {
            QString error = "Unable to open file with SFTP: " + remotePath;
            qDebug() << error;
            throw FileTransferException(error);
        }
        CHECK_CANCEL();
    } while (!sftp_handle);

    CResourceDestructor sftpHandleDestructor([sftp_handle]() {
            libssh2_sftp_close(sftp_handle);
    });

    if (this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
        QFileInfo fileInfo(localFilename);

        if (fileInfo.exists()) {
            if (this->fileOverwriteAnswer == FileOverwriteAnswer::NoToAll) {
                return;
            }

            CHECK_CANCEL();
            this->waitUntilFileOverwriteAnswerChanged("Download from "
                    + this->job->getConnEntry()->getIdentifier(),
                    "Local file '" + localFilename + "' already exists!",
                    "Do you want to overwrite this file?");
            CHECK_CANCEL();

            if (this->fileOverwriteAnswer != FileOverwriteAnswer::Yes
                    && this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
                return;
            }
        }
    }

    CHECK_CANCEL();
    fp = fopen(localFilename.toLatin1().data(), "w");
    if (!fp) {
        errorString = "Can't open local file: " + localFilename;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    do {
        while ((nread = libssh2_sftp_read(sftp_handle, this->transferBuffer, sizeof(this->transferBuffer))) ==
                LIBSSH2_ERROR_EAGAIN) {
            CHECK_CANCEL();
            QThread::msleep(this->sleeptime);
            CHECK_CANCEL();
        }

        if (nread <= 0) {
            /* end of file */
            break;
        }

        ptr = this->transferBuffer;
        this->updateTransferSpeed(nread);

        do {
            rc = fwrite(ptr, 1, nread, fp);
            ptr += rc;
            nread -= rc;

            this->job->bytesTransferred += rc;
        } while (nread);

        CHECK_CANCEL();
    } while (rc > 0);

    fclose(fp);

    // set file permissions
    while ((rc = libssh2_sftp_fstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        CHECK_CANCEL();
        QThread::msleep(this->sleeptime);
        CHECK_CANCEL();
    }

    if (rc != 0) {
        errorString = "Failed stat over SFTP: " + remotePath;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    if (chmod(localFilename.toLatin1().data(), attrs.permissions) != 0) {
        errorString = "Failed chmod for file: " + localFilename;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    qDebug() << "Finished downloading file " << remotePath;
}

void FileTransferWorker::copyFileToRemote(QString localPath, QString remoteDir)
{
    int rc = 0;
    int nread = 0;
    char *ptr = NULL;
    FILE *fp = NULL;
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    QString errorString;
    struct stat statbuf;
    QFile file(localPath);
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    if (this->conn == nullptr) {
        throw FileTransferException("No connection");
    }

    qDebug() << "Uploading file " << localPath;

    fp = fopen(localPath.toLatin1().data(), "rb");
    if (!fp) {
        QString error = "Can't open local file: " + localPath;
        qDebug() << error;
        throw FileTransferException(error);
    }

    QString remoteFilename = remoteDir + "/" + Util::basename(localPath);

    if (this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
        CHECK_CANCEL();
        if (this->sftpFileExists(remoteFilename)) {
            if (this->fileOverwriteAnswer == FileOverwriteAnswer::NoToAll) {
                return;
            }

            CHECK_CANCEL();
            this->waitUntilFileOverwriteAnswerChanged("Upload to "
                    + this->job->getConnEntry()->getIdentifier(),
                    "Remote file '" + remoteFilename + "' already exists!",
                    "Do you want to overwrite this file?");

            if (this->fileOverwriteAnswer != FileOverwriteAnswer::Yes
                    && this->fileOverwriteAnswer != FileOverwriteAnswer::YesToAll) {
                return;
            }
        }
    }

    do {
        CHECK_CANCEL();
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remoteFilename.toLatin1().data(),
                LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
        CHECK_CANCEL();

        if (!sftp_handle &&
                (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {

            QString error = "Unable to open file '" + remoteFilename + "' with SFTP";
            qDebug() << error;
            throw FileTransferException(error);
        }
    } while (!sftp_handle);

    CResourceDestructor sftpHandleDestructor([sftp_handle]() {
            libssh2_sftp_close(sftp_handle);
    });

    do {
        nread = fread(this->transferBuffer, 1, sizeof(this->transferBuffer), fp);
        if (nread <= 0) {
            /* end of file */
            break;
        }

        ptr = this->transferBuffer;
        this->updateTransferSpeed(nread);

        do {
            /* write data in a loop until we block */
            while ((rc = libssh2_sftp_write(sftp_handle, ptr, nread)) ==
                    LIBSSH2_ERROR_EAGAIN) {
                CHECK_CANCEL();
                QThread::msleep(this->sleeptime);
                CHECK_CANCEL();
            }
            if(rc < 0)
                break;
            ptr += rc;
            nread -= rc;
            this->job->bytesTransferred += rc;
        } while (nread);

        CHECK_CANCEL();
    } while (rc > 0);

    // set file permissions
    if (stat(localPath.toLatin1().data(), &statbuf) != 0) {
        errorString = "Failed stat for file: " + localPath;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    while ((rc = libssh2_sftp_fstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        CHECK_CANCEL();
        QThread::msleep(this->sleeptime);
        CHECK_CANCEL();
    }

    if (rc != 0) {
        errorString = "Failed stat over SFTP: " + remoteFilename;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    attrs.permissions = statbuf.st_mode;

    while ((libssh2_sftp_fsetstat(sftp_handle, &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        CHECK_CANCEL();
        QThread::msleep(this->sleeptime);
        CHECK_CANCEL();
    }

    if (rc != 0) {
        errorString = "Failed change permissions over SFTP: " + remoteFilename;
        qDebug() << errorString;
        throw FileTransferException(errorString);
    }

    qDebug() << "Finished uploading file " << localPath;
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

void FileTransferWorker::connectWithThread(QThread *thread)
{
    thread->connect(thread, &QThread::started, this, &FileTransferWorker::process);
    thread->connect(this, &FileTransferWorker::finished, thread, &QThread::quit);
    this->connect(this, &FileTransferWorker::finished, this, &FileTransferWorker::deleteLater);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
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

bool FileTransferWorker::sftpFileExists(QString filename)
{
    int rc;
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    while ((rc = libssh2_sftp_stat(this->conn->sftp, filename.toLatin1().data(), &attrs)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(this->conn);
    }

    if (rc != 0) {
        int err = libssh2_sftp_last_error(this->conn->sftp);
        if (err == LIBSSH2_FX_NO_SUCH_FILE) {
            return false;
        }

        QString error = "Failed stat for sftpFileExists: " + filename;
        qDebug() << error;
        throw FileTransferException(error);
    }

    return true;
}

void FileTransferWorker::updateTransferSpeed(uint64_t bytediff)
{
    qint64 elapsed = this->transferTimer.elapsed();
    qint64 timediff = elapsed - this->lastTransferTime;

    this->accumulatedByteDiff += bytediff;

    if (timediff < 1000) {
        return;
    }

    uint64_t bytesPerSecond = (1000.0 / timediff) * this->accumulatedByteDiff;

    this->accumulatedByteDiff = 0;
    this->lastTransferTime = elapsed;
    this->job->setBytesPerSecond(bytesPerSecond);
}
