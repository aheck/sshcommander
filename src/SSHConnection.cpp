#include "SSHConnection.h"

SSHConnection::SSHConnection()
{
    this->socket_fd = 0;
    this->session = nullptr;
}

SSHConnection::~SSHConnection()
{
    if (session == nullptr) {
        return;
    }

    libssh2_session_disconnect(session, "Normal disconnect of SSH Commander");
    libssh2_session_free(session);
}
