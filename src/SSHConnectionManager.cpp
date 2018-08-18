#include "SSHConnectionManager.h"

SSHConnectionManager& SSHConnectionManager::getInstance()
{
    static SSHConnectionManager connectionManager;
    return connectionManager;
}

SSHConnectionManager::SSHConnectionManager()
{
    int status = libssh2_init(0);

    if (status != 0) {
        std::cerr << "SSHConnectionManager: libssh2 initialization failed with error code " << status << std::endl;
    }

    qRegisterMetaType<std::shared_ptr<RemoteCmdResult>>();
}

SSHConnectionManager::~SSHConnectionManager()
{
    QStringList keysToRemove;

    // tell all file transfer jobs they are canceled
    for (auto const& jobs : this->fileTransferJobs) {
        for (auto const& job : jobs.second) {
            job->cancelationRequested = true;
        }
    }

    // wait for all file transfer jobs to terminate
    for (auto const& jobs : this->fileTransferJobs) {
        for (auto const& job : jobs.second) {
            while (!job->isDone()) {
                QThread::msleep(50);
            }
        }

        keysToRemove.append(jobs.first);
    }

    // remove all file transfer jobs from the std::map this->fileTransferJobs
    for (QString const& key : keysToRemove) {
        this->fileTransferJobs.erase(key);
    }

    libssh2_exit();
}

uint64_t SSHConnectionManager::executeRemoteCmd(std::shared_ptr<SSHConnectionEntry> connEntry,
        QString cmd, QObject *slotObject, const char *slot)
{
    uint64_t requestId = this->generateRequestId();

    std::thread([this, connEntry, cmd, slotObject, slot]
    {
        connEntry->connectionMutex.lock();
        std::shared_ptr<RemoteCmdResult> result;
        if (connEntry->connection == nullptr) {
            connEntry->connection = this->createSSHConnection(connEntry);
        }

        if (connEntry->connection != nullptr) {
            result = this->doExecuteRemoteCmd(connEntry->connection, cmd);
        } else {
            result = std::make_shared<RemoteCmdResult>();
            result->isSuccess = false;
            result->errorString = "Failed to establish connection to host: " + connEntry->hostname;
        }

        QMetaObject::invokeMethod(slotObject, slot, Q_ARG(std::shared_ptr<RemoteCmdResult>, result));
        connEntry->connectionMutex.unlock();
    }).detach();

    return requestId;
}

std::shared_ptr<SSHConnection> SSHConnectionManager::createSSHConnection(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    LIBSSH2_SESSION *session = NULL;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    int sock;
    int retval;

    auto conn = std::make_shared<SSHConnection>();
    QString hostname;

    hostname = connEntry->hostname;
    QHostInfo hostInfo = QHostInfo::fromName(hostname);
    if (hostInfo.addresses().count() == 0) {
        return nullptr;
    }

    QHostAddress address = hostInfo.addresses().first();

    std::cout << "Trying to connect to " << address.toString().toStdString() << std::endl;

    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(connEntry->port);
        sin.sin_addr.s_addr = htonl(address.toIPv4Address());
        if (::connect(sock, (struct sockaddr*)(&sin),
                    sizeof(struct sockaddr_in)) != 0) {
            std::cerr << "SSHConnectionManager: Failed to connect to "
                << hostname.toStdString() << "(" << address.toString().toStdString() << ")" << std::endl;
            return nullptr;
        }
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        sin6.sin6_family = AF_INET6;
        sin6.sin6_port = htons(connEntry->port);
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        memcpy(&(sin6.sin6_addr), &ipv6, sizeof(ipv6));
        if (::connect(sock, (struct sockaddr*)(&sin6),
                    sizeof(struct sockaddr_in6)) != 0) {
            std::cerr << "SSHConnectionManager: Failed to connect to "
                << hostname.toStdString() << "(" << address.toString().toStdString() << ")" << std::endl;
            return nullptr;
        }
    } else {
        std::cerr << "createSSHConnection: Error, address is neither IPv4 nor IPv6!\n";
        return nullptr;
    }

    conn->socket_fd = sock;
 
    session = libssh2_session_init();
    conn->session = session;

    if (!session) {
        return nullptr;
    }
 
    while ((retval = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN);

    if (retval) {
        std::cerr << "SSHConnectionManager: Failed to establish SSH session: "
            <<  retval << std::endl;
        return nullptr;
    }

    // FIXME: Implement known host checking

    QByteArray username = connEntry->username.toLatin1();
    QByteArray password;

    if (!connEntry->password.isEmpty()) {
        password = connEntry->password.toLatin1();

        while ((retval = libssh2_userauth_password(session, username.data(), password.data())) == LIBSSH2_ERROR_EAGAIN);
        if (retval) {
            std::cerr << "SSHConnectionManager: Authentication by password failed." << std::endl;
            return nullptr;
        }
    } else {
        QString sshkey = connEntry->sshkey;
        if (sshkey.isEmpty()) {
            sshkey = QDir(QDir::home().filePath(".ssh")).filePath("id_rsa");
        }
        QByteArray privateCertPath = sshkey.toLatin1();
        while ((retval = libssh2_userauth_publickey_fromfile(session, username,
                        NULL, privateCertPath.data(), "")) == LIBSSH2_ERROR_EAGAIN);
        if (retval) {
            std::cerr << "SSHConnectionManager: Authentication by public key failed" << std::endl;
            return nullptr;
        }
    }

    libssh2_session_set_blocking(session, 0);

    return conn;
}

std::shared_ptr<RemoteCmdResult> SSHConnectionManager::doExecuteRemoteCmd(std::shared_ptr<SSHConnection> conn, QString cmd)
{
    int retval;
    int bytes = 0;
    int exitcode;
    char *exitsignal = (char *) "none";
    LIBSSH2_CHANNEL *channel = NULL;

    auto result = std::make_shared<RemoteCmdResult>();
    result->command = cmd;
    result->isSuccess = false;

    while((channel = libssh2_channel_open_session(conn->session)) == NULL &&
           libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(conn);
    }

    if (channel == NULL) {
        std::cerr << "Failed to create channel" << std::endl;
        return result;
    }

    while((retval = libssh2_channel_exec(channel, cmd.toLatin1().data())) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(conn);
    }

    if (retval != 0) {
        std::cerr << "Failed to execute command: " << cmd.toStdString() << std::endl;
        return result;
    }

    // read the output of the command
    while (1) {
        /* loop until we block */ 
        int retval;
        do {
            char buffer[4096];
            retval = libssh2_channel_read(channel, buffer, sizeof(buffer));

            if (retval > 0) {
                bytes += retval;
                result->resultString.append(QString::fromUtf8(buffer, retval));
            } else {
                if(retval != LIBSSH2_ERROR_EAGAIN) {
                    std::cerr << "libssh2_channel_read returned: " << retval << std::endl;
                }
            }
        } while(retval > 0);
 
        /* this is due to blocking that would occur otherwise so we loop on
           this condition */ 
        if( retval == LIBSSH2_ERROR_EAGAIN ) {
            SSHConnectionManager::waitsocket(conn);
        } else {
            break;
        }
    }

    exitcode = 127;
    while((retval = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN) {
        SSHConnectionManager::waitsocket(conn);
    }
 
    if (retval == 0) {
        exitcode = libssh2_channel_get_exit_status(channel);

        libssh2_channel_get_exit_signal(channel, &exitsignal, NULL, NULL, NULL, NULL, NULL);
    }
 
    if (exitsignal)
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
    else
        fprintf(stderr, "\nEXIT: %d bytes: %d\n", exitcode, bytes);
 
    libssh2_channel_free(channel);

    channel = NULL;

    result->isSuccess = true;
    result->statusCode = exitcode;

    return result;
}

std::vector<std::shared_ptr<DirEntry>> SSHConnectionManager::readDirectory(std::shared_ptr<SSHConnectionEntry> connEntry, QString dir, bool onlyDirs)
{
    connEntry->connectionMutex.lock();
    auto entries = this->doReadDirectory(connEntry->connection, dir, onlyDirs);
    connEntry->connectionMutex.unlock();

    return entries;
}

void SSHConnectionManager::addFileTransferJob(std::shared_ptr<FileTransferJob> job)
{
    auto connEntry = job->getConnEntry();
    this->fileTransferJobs[connEntry->getIdentifier()].push_back(job);

    this->executeFileTransfer(job);
}

void SSHConnectionManager::executeFileTransfer(std::shared_ptr<FileTransferJob> job)
{
    QThread *thread = new QThread();
    job->setThread(thread);
    FileTransferWorker *worker = new FileTransferWorker(job);
    worker->moveToThread(thread);

    // connect slots for message box handling in the main thread
    thread->connect(worker, SIGNAL(askToOverwriteFile(QString, QString, QString)), this, SLOT(askToOverwriteFile(QString, QString, QString)));

    thread->connect(thread, SIGNAL(started()), worker, SLOT(process()));
    thread->connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    thread->connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    thread->connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

std::vector<std::shared_ptr<DirEntry>> SSHConnectionManager::doReadDirectory(std::shared_ptr<SSHConnection> conn, QString dir, bool onlyDirs)
{
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    std::vector<std::shared_ptr<DirEntry>> entries;

    // FIXME: Try to create connection if conn == nullptr
    if (conn == nullptr) {
        return entries;
    }

    if (conn->sftp == nullptr) {
        while ((conn->sftp = libssh2_sftp_init(conn->session)) == nullptr &&
                libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN)  {
            SSHConnectionManager::waitsocket(conn);
        }

        if (conn->sftp == nullptr) {
            std::cerr << "Unable to init SFTP session" << libssh2_session_last_error(conn->session,NULL,NULL,0) << "\n";
            return entries;
        }
    }

    while ((sftp_handle = libssh2_sftp_opendir(conn->sftp, dir.toLatin1().data())) == nullptr &&
            libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN) {
        waitsocket(conn);
    }

    if (sftp_handle == nullptr) {
        std::cerr << "Failed to open directory" << dir.toLatin1().data() << "\n";
        return entries;
    }

    do {
        char mem[512];
        char longentry[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;
        int rc;

        while ((rc = libssh2_sftp_readdir_ex(sftp_handle, mem, sizeof(mem), longentry, sizeof(longentry), &attrs)) <= 0 &&
                libssh2_session_last_error(conn->session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN) {
            std::cerr << "Waiting for socket..\n";
            libssh2_session_set_last_error(conn->session, LIBSSH2_ERROR_NONE, "");
            SSHConnectionManager::waitsocket(conn);
        }

        if (rc > 0) {
            if (strcmp(".", mem) == 0) {
                continue;
            }

            if (strcmp("..", mem) == 0) {
                continue;
            }

            if (onlyDirs) {
                if (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS && !LIBSSH2_SFTP_S_ISDIR(attrs.permissions)) {
                    continue;
                }
            }

            auto dirEntry = std::make_shared<DirEntry>();
            dirEntry->setPath(dir);
            dirEntry->setFilename(mem);

            dirEntry->setLongEntry(longentry);

            if (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) {
                dirEntry->setPermissions(attrs.permissions);
            }

            if (attrs.flags & LIBSSH2_SFTP_ATTR_UIDGID) {
                dirEntry->setUid(attrs.uid);
                dirEntry->setGid(attrs.gid);
            }

            if (attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
                dirEntry->setFilesize(attrs.filesize);
            }

            if (attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME) {
                dirEntry->setAtime(attrs.atime);
                dirEntry->setMtime(attrs.mtime);
            }

            entries.push_back(dirEntry);
        }
        else {
            break;
        }
    } while (1);

    libssh2_sftp_closedir(sftp_handle);

    return entries;
}

uint64_t SSHConnectionManager::generateRequestId()
{
    this->requestIdMutex.lock();
    uint64_t result = this->nextRequestId++;
    this->requestIdMutex.unlock();

    return result;
}

int SSHConnectionManager::waitsocket(std::shared_ptr<SSHConnection> conn)
{
    struct timeval timeout;
    int retval;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;
 
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
 
    FD_ZERO(&fd);
 
    FD_SET(conn->socket_fd, &fd);
 
    dir = libssh2_session_block_directions(conn->session);

    if (dir & LIBSSH2_SESSION_BLOCK_INBOUND) {
        readfd = &fd;
    }
 
    if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) {
        writefd = &fd;
    }
 
    retval = select(conn->socket_fd + 1, readfd, writefd, NULL, &timeout);
 
    return retval;
}

int SSHConnectionManager::countFileTransferJobs(QString connectionId)
{
    return this->fileTransferJobs[connectionId].size();
}

std::shared_ptr<FileTransferJob> SSHConnectionManager::getFileTransferJob(QString connectionId, int row)
{
    if (row < 0 || row > this->fileTransferJobs[connectionId].size() - 1) {
        return nullptr;
    }

    return this->fileTransferJobs[connectionId][row];
}

bool SSHConnectionManager::removeFileTransferJob(QString connectionId, int row)
{
    if (this->fileTransferJobs.count(connectionId) == 0) {
        return false;
    }

    if (this->fileTransferJobs[connectionId].size() < (row + 1)) {
        return false;
    }

    this->fileTransferJobs[connectionId][row]->cancelationRequested = true;
    this->fileTransferJobs[connectionId].erase(this->fileTransferJobs[connectionId].begin() + row);
    return true;
}

int SSHConnectionManager::getFileTransferJobRowByUuid(QString connectionId, QUuid uuid)
{
    int i = 0;
    for (auto job : this->fileTransferJobs[connectionId]) {
        if (job->getUuid() == uuid) {
            return i;
        }

        i++;
    }

    return -1;
}

void SSHConnectionManager::askToOverwriteFile(QString title, QString message, QString infoText)
{
    FileTransferWorker *worker = qobject_cast<FileTransferWorker*>(sender());

    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setInformativeText(infoText);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        worker->setFileOverwriteAnswerAndNotify(FileOverwriteAnswer::Yes);
    } else if (ret == QMessageBox::No) {
        worker->setFileOverwriteAnswerAndNotify(FileOverwriteAnswer::No);
    } else if (ret == QMessageBox::YesToAll) {
        worker->setFileOverwriteAnswerAndNotify(FileOverwriteAnswer::YesToAll);
    } else if (ret == QMessageBox::NoToAll) {
        worker->setFileOverwriteAnswerAndNotify(FileOverwriteAnswer::NoToAll);
    }
}
