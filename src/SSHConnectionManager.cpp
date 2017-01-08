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

    sock = socket(AF_INET, SOCK_STREAM, 0);
    conn->socket_fd = sock;
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(connEntry->port);
    sin.sin_addr.s_addr = htonl(address.toIPv4Address());
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        std::cerr << "SSHConnectionManager: Failed to connect to "
            << hostname.toStdString() << "(" << address.toString().toStdString() << ")" << std::endl;
        return nullptr;
    }
 
    session = libssh2_session_init();
    conn->session = session;

    if (!session) {
        return nullptr;
    }

    libssh2_session_set_blocking(session, 0);
 
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
        QByteArray privateCertPath = connEntry->sshkey.toLatin1();
        while ((retval = libssh2_userauth_publickey_fromfile(session, username,
                        NULL, privateCertPath.data(), "")) == LIBSSH2_ERROR_EAGAIN);
        if (retval) {
            std::cerr << "SSHConnectionManager: Authentication by public key failed" << std::endl;
            return nullptr;
        }
    }

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
        this->waitsocket(conn);
    }

    if (channel == NULL) {
        std::cerr << "Failed to create channel" << std::endl;
        return result;
    }

    while((retval = libssh2_channel_exec(channel, cmd.toLatin1().data())) == LIBSSH2_ERROR_EAGAIN) {
        this->waitsocket(conn);
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
            this->waitsocket(conn);
        } else {
            break;
        }
    }

    exitcode = 127;
    while((retval = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN ) {
        this->waitsocket(conn);
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
 
    timeout.tv_sec = 10;
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