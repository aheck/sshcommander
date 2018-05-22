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
    if (this->session != nullptr) {
        libssh2_session_disconnect(this->session, "Normal disconnect of SSH Commander");
        libssh2_session_free(this->session);
    }

    if (this->sftp != nullptr) {
        libssh2_sftp_shutdown(this->sftp);
    }

    if (this->sftp_session != nullptr) {
        libssh2_session_free(this->sftp_session);
    }
}
