#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QColor>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QFont>

#include <qtermwidget.h>

class Preferences : public QObject
{
    Q_OBJECT

public:
    Preferences();

    void read();
    void save();

    QFont getDefaultTerminalFont();

    QFont getTerminalFont();
    void setTerminalFont(const QFont &font);

    const QString getAWSAccessKey();
    void setAWSAccessKey(const QString accessKey);
    const QString getAWSSecretKey();
    void setAWSSecretKey(const QString secretKey);

private:
    bool hasDefaultTerminalFont;
    QFont defaultTerminalFont;
    QFont terminalFont;
    QString awsAccessKey;
    QString awsSecretKey;
};

#endif
