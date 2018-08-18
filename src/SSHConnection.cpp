#include "SSHConnection.h"

SSHConnection::SSHConnection()
{
    this->socket_fd = 0;
    this->session = nullptr;
    this->sftp = nullptr;
    this->alive = false;
}

SSHConnection::~SSHConnection()
{
    if (this->sftp != nullptr) {
        libssh2_sftp_shutdown(this->sftp);
    }

    if (this->session != nullptr) {
        libssh2_session_disconnect(this->session, "Normal disconnect of SSH Commander");
        libssh2_session_free(this->session);
    }
}

bool SSHConnection::isAlive()
{
    return alive;
}

void SSHConnection::declareDead()
{
    this->alive = false;
}
