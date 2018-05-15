#include "SSHConnection.h"

SSHConnection::SSHConnection()
{
    this->socket_fd = 0;
    this->session = nullptr;
    this->sftp = nullptr;
    this->sftp_session = nullptr;
}

SSHConnection::~SSHConnection()
{
    if (session == nullptr) {
        return;
    }

    libssh2_session_disconnect(this->session, "Normal disconnect of SSH Commander");
    libssh2_session_free(this->session);
    libssh2_sftp_shutdown(this->sftp);
    libssh2_session_free(this->sftp_session);
}
