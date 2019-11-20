/*****************************************************************************
 *
 * PseudoTerminal allows to run and control terminal-based programs in
 * headless mode.
 *
 ****************************************************************************/

#ifndef PSEUDOTERMINAL_H
#define PSEUDOTERMINAL_H

#include <QCoreApplication>
#include <QDateTime>
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
    bool waitForFinished(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);
    QString readAllOutput();

signals:
    void finished(int exitCode);
    void dataReceived(const QString data);
    void readReady();
    void lineReceived(const QString line);
    void errorOccured(QProcess::ProcessError, QString message);

private slots:
    void readFd(int fd);

private:
    pid_t childPid;
    int masterFd;
    int slaveFd;
    int _statusCode;
    QString lineBuffer;
    QString readBuffer;
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
