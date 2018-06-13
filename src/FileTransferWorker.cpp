#include "FileTransferWorker.h"

FileTransferWorker::FileTransferWorker(std::shared_ptr<FileTransferJob> job)
{
    this->job = job;
}

FileTransferWorker::~FileTransferWorker()
{

}

void FileTransferWorker::process()
{
    this->job->setState(FileTransferState::Connecting);

    auto connEntry = this->job->getConnEntry();
    this->conn = SSHConnectionManager::getInstance().createSSHConnection(connEntry);

    if (conn->sftp == nullptr) {
        while ((conn->sftp = libssh2_sftp_init(conn->session)) == nullptr &&
                libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN)  {
            SSHConnectionManager::waitsocket(conn);
        }

        if (conn->sftp == nullptr) {
            std::cerr << "Unable to init SFTP session" << libssh2_session_last_error(conn->session,NULL,NULL,0) << "\n";
            this->job->setState(FileTransferState::FailedConnect);
            return;
        }
    }

    if (conn == nullptr) {
        this->job->setState(FileTransferState::Failed);
        emit finished();
    }

    this->job->setState(FileTransferState::Running);

    for (QString const &filename : job->getFilesToCopy()) {
        if (job->getType() == FileTransferType::Download) {
            this->copyFileFromRemote(filename, job->getTargetDir());
        } else {
            this->copyFileToRemote(filename, job->getTargetDir());
        }
    }

    this->job->setState(FileTransferState::Completed);
    emit finished();
}

void FileTransferWorker::copyFileFromRemote(QString remotePath, QString localDir)
{
    int rc = 0;
    int nread = 0;
    char *ptr = NULL;
    FILE *fp = NULL;
    char buffer[1024*4];
    LIBSSH2_SFTP_HANDLE *sftp_handle = NULL;
    uint64_t total = 0;

    std::cerr << "Downloading file " << remotePath.toStdString() << "\n";

    if (this->conn == nullptr) {
        return;
    }

    QString localFilename = localDir + "/" + Util::basename(remotePath);

    do {
        std::cerr << "Trying to open file '" << remotePath.toStdString() << "' via SFTP...\n";
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remotePath.toLatin1().data(),
                LIBSSH2_FXF_READ, 0);

        if (!sftp_handle && (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {
            fprintf(stderr, "Unable to open file with SFTP\n");
            goto shutdown;
        }
    } while (!sftp_handle);

    fp = fopen(localFilename.toLatin1().data(), "w");
    if (!fp) {
        fprintf(stderr, "Can't open local file %s\n", localFilename.toLatin1().data());
        return;
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

    std::cerr << "Finished downloading file " << remotePath.toStdString() << "\n";

shutdown:
    libssh2_sftp_close(sftp_handle);
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

    if (this->conn == nullptr) {
        return;
    }

    std::cerr << "Uploading file " << localPath.toStdString() << "\n";

    fp = fopen(localPath.toLatin1().data(), "rb");
    if (!fp) {
        fprintf(stderr, "Can't open local file %s\n", localPath.toLatin1().data());
        return;
    }

    QString remoteFilename = remoteDir + "/" + Util::basename(localPath);

    do {
        sftp_handle = libssh2_sftp_open(this->conn->sftp, remoteFilename.toLatin1().data(),
                LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

        if (!sftp_handle &&
                (libssh2_session_last_errno(this->conn->session) != LIBSSH2_ERROR_EAGAIN)) {

            fprintf(stderr, "Unable to open file with SFTP\n");
            goto shutdown;
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

shutdown:
    libssh2_sftp_close(sftp_handle);

    std::cerr << "Finished uploading file " << localPath.toStdString() << "\n";
}
