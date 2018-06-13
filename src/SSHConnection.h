/*****************************************************************************
 *
 * SSHConnection is a container for a libssh2 connection as well as locks and
 * everything that belongs to the connection.
 *
 * It is primarily used as a member of an SSHConnectionEntry. Access is only
 * allowed to SSHConnectionManager, though because it is responsible for
 * handling all libssh2 connections and for managing the threads used for
 * serving requests to those connection objects.
 *
 ****************************************************************************/

#ifndef SSHCONNECTION_H
#define SSHCONNECTION_H

#include <libssh2.h>
#include <libssh2_sftp.h>

class SSHConnection
{
public:
    friend class SSHConnectionManager;
    friend class FileTransferWorker;

    SSHConnection();
    ~SSHConnection();

private:
    int socket_fd;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp;
};

#endif
