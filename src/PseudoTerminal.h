/*****************************************************************************
 *
 * PseudoTerminal allows to run and control terminal-based programs in
 * headless mode.
 *
 ****************************************************************************/

#ifndef PSEUDOTERMINAL_H
#define PSEUDOTERMINAL_H

#include <QList>
#include <QProcess>
#include <QSocketNotifier>
#include <QString>
#include <QThread>

#include <termios.h>
#include <iostream>
#include <signal.h>

class PseudoTerminal : public QObject
{
    Q_OBJECT

public:
    PseudoTerminal();
    ~PseudoTerminal();

    void start(const QString &command, const QStringList &args = QStringList());
    bool isRunning();
    void sendData(const QString &data);
    void terminate(int secsToForce = 10);
    int statusCode();

signals:
    void finished(int exitCode);
    void dataReceived(const QString data);
    void lineReceived(const QString line);
    void errorOccured(QProcess::ProcessError, QString message);

public slots:
    void readReady(int fd);

private:
    pid_t childPid;
    int masterFd;
    int slaveFd;
    int _statusCode;
    QString buffer;
    QString command;
    QStringList args;
    QSocketNotifier *fdWatcher;

    bool readFromTerminal(char *buf, int bufSize);
    void appendToLineBuffer(char *buf, int bufSize);
    int ptyMasterOpen(char *slaveName, size_t snLen);
    pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
            const struct termios *slaveTermios, const struct winsize *slaveWS);
};

#endif
