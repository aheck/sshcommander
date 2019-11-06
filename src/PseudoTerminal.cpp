#include <PseudoTerminal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
#define MAX_SNAME 1024

static struct termios ttyOrig;

PseudoTerminal::PseudoTerminal()
{
    this->fdWatcher = nullptr;
    this->_statusCode = 0;
    this->childPid = -1;
    this->masterFd = -1;
    this->slaveFd = -1;
}

PseudoTerminal::~PseudoTerminal()
{
}

void PseudoTerminal::start(const QString &command, const QStringList &args)
{
    char slaveName[MAX_SNAME];
    struct winsize ws;
    const char **arglist;

    this->command = command;
    this->args = args;

    // save original terminal attributes
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1) {
        fprintf(stderr, "Failed to save original terminal attributes\n");
        return;
    }

    // get winsize
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws)) {
        fprintf(stderr, "Failed to get terminal winsize\n");
        return;
    }

    this->childPid = ptyFork(&this->masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
    if (this->childPid == -1) {
        fprintf(stderr, "Failed to fork child\n");
        return;
    }

    if (this->childPid == 0) {
        // Child
        int size = args.length() + 2;
        arglist = new const char*[size];
        arglist[0] = command.toStdString().c_str();
        int i = 1;
        for (const QString &arg: args) {
            arglist[i] = arg.toStdString().c_str();
            i++;
        }

        arglist[size - 1] = nullptr;

        execv(command.toLatin1().data(), (char* const*) arglist);

        fprintf(stderr, "Failed to start shell\n");
        exit(0);
    }

    // Parent
    this->fdWatcher = new QSocketNotifier(this->masterFd, QSocketNotifier::Read, this);
    connect(this->fdWatcher, &QSocketNotifier::activated, this, &PseudoTerminal::readReady);
}

void PseudoTerminal::readReady(int fd)
{
    char buf[BUF_SIZE];

    int numRead = read(this->masterFd, buf, BUF_SIZE - 1);

    if (numRead <= 0) {
        if (numRead == -1) {
            if (errno == EIO) {
                this->fdWatcher->setEnabled(0);
            }
            else {
                perror("PseudoTerminal.cpp: ");
            }
        }

        int result;
        pid_t pid = waitpid(this->childPid, &result, WNOHANG);
        if (pid > 0) {
            this->_statusCode = WEXITSTATUS(result);
            std::cout << "statusCode: " << this->_statusCode << "\n";
            emit finished(this->_statusCode);
        }
        return;
    }

    buf[numRead] = '\0';
    emit(dataReceived(QString(buf)));

    this->buffer.append(buf);
    int pos;
    while (1) {
        pos = this->buffer.indexOf('\n');
        if (pos == -1) {
            break;
        }
        QString line = this->buffer.left(pos + 1);
        this->buffer.remove(0, pos + 1);
        emit lineReceived(line);
    };
}

bool PseudoTerminal::isRunning()
{
    if (this->childPid <= 0) {
        // No valid childPid: process hasn't even been started, yet
        return false;
    }

    int result;
    pid_t pid = waitpid(this->childPid, &result, WNOHANG);
    if (pid > 0) {
        // child has terminated
        return false;
    }

    return true;
}

int PseudoTerminal::statusCode()
{
    return this->_statusCode;
}

void PseudoTerminal::sendData(const QString &data)
{
    QByteArray ba = data.toUtf8();
    write(this->masterFd, ba.data(), ba.length());
}

void PseudoTerminal::terminate(int secsToForce)
{
    const int msecsToWait = 10;

    if (!this->isRunning()) {
        return;
    }

    int msecsPassed = 0;

    kill(this->childPid, SIGTERM);
    while (1) {
        QThread::msleep(msecsToWait);
        msecsPassed += msecsToWait;

        if (msecsPassed >= secsToForce * 1000) {
            kill(this->childPid, SIGKILL);
        }

        int result;
        pid_t pid = waitpid(this->childPid, &result, WNOHANG);
        if (pid == this->childPid) {
            this->_statusCode = WEXITSTATUS(result);
            emit finished(this->_statusCode);
            return;
        }

        if (!this->isRunning()) {
            return;
        }
    }
}

int PseudoTerminal::ptyMasterOpen(char *slaveName, size_t snLen)
{
    int masterFd, savedErrno;
    char *p;

    masterFd = posix_openpt(O_RDWR | O_NOCTTY);
    if (masterFd == -1) {
        return -1;
    }

    if (grantpt(masterFd) == -1) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;

        return -1;
    }

    if (unlockpt(masterFd) == -1) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;

        return -1;
    }

    p = ptsname(masterFd);
    if (p == NULL) {
        savedErrno = errno;
        close(masterFd);
        errno = savedErrno;

        return -1;
    }

    if (strlen(p) < snLen) {
        strncpy(slaveName, p, snLen);
    } else {
        close(masterFd);
        errno = EOVERFLOW;

        return -1;
    }

    return masterFd;
}

pid_t PseudoTerminal::ptyFork(int *masterFd, char *slaveName, size_t snLen,
        const struct termios *slaveTermios, const struct winsize *slaveWS)
{
    int mfd, savedErrno;
    pid_t childPid;
    char slname[MAX_SNAME];

    mfd = ptyMasterOpen(slname, MAX_SNAME);
    if (mfd == -1) {
        return -1;
    }

    if (slaveName != NULL) {
        if (strlen(slname) < snLen) {
            strncpy(slaveName, slname, snLen);
        } else {
            close(mfd);
            errno = EOVERFLOW;

            return -1;
        }
    }

    childPid = fork();

    if (childPid == -1) {
        savedErrno = errno;
        close(mfd);
        errno = savedErrno;

        return -1;
    }

    if (childPid != 0) {
        // Parent
        this->masterFd = mfd;
        return childPid;
    }

    // Child
    if (setsid() == -1) {
        fprintf(stderr, "Failed setsid\n");
        return -1;
    }

    close(mfd);

    this->slaveFd = open(slname, O_RDWR);
    if (this->slaveFd == -1) {
        fprintf(stderr, "Failed to open slave\n");
        return -1;
    }

#ifdef TIOSCSCTTY
    if (ioctl(this->slaveFd, TIOCSCTTY, 0) == -1) {
        fprintf(stderr, "Failed TIOCSCTTY on slave\n");
        return -1;
    }
#endif

    if (slaveTermios != NULL) {
        if (tcsetattr(this->slaveFd, TCSANOW, slaveTermios) == -1) {
            fprintf(stderr, "Failed TCSANOW on slave\n");
            return -1;
        }
    }

    if (slaveWS != NULL) {
        if (ioctl(this->slaveFd, TIOCSWINSZ, slaveWS) == -1) {
            fprintf(stderr, "Failed TIOCSWINSZ on slave\n");
            return -1;
        }
    }

    // Duplicate pty slave fd to be child's stdin, stdout and stderr
    if (dup2(this->slaveFd, STDIN_FILENO) != STDIN_FILENO) {
        fprintf(stderr, "Failed dup of STDIN\n");
        return -1;
    }

    if (dup2(this->slaveFd, STDOUT_FILENO) != STDOUT_FILENO) {
        fprintf(stderr, "Failed dup of STDOUT\n");
        return -1;
    }

    if (dup2(this->slaveFd, STDERR_FILENO) != STDERR_FILENO) {
        fprintf(stderr, "Failed dup of STDERR\n");
        return -1;
    }

    if (this->slaveFd > STDERR_FILENO) {
        close(this->slaveFd);
        this->slaveFd = -1;
    }

    return 0;
}
