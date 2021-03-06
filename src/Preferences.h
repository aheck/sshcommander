/*****************************************************************************
 *
 * Preferences is the model behind the PreferencesDialog.
 *
 * It manages all user editable preferences.
 *
 ****************************************************************************/

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
    static Preferences& getInstance();

    Preferences(Preferences const &other) = delete;
    void operator=(Preferences const &) = delete;

    void read();
    void save();

    QFont getDefaultTerminalFont();

    QFont getTerminalFont();
    void setTerminalFont(const QFont &font);

    const QString getAWSAccessKey();
    void setAWSAccessKey(const QString accessKey);
    const QString getAWSSecretKey();
    void setAWSSecretKey(const QString secretKey);

    const QString getColorScheme();
    void setColorScheme(const QString colorScheme);

private:
    Preferences();
    ~Preferences();

    bool hasDefaultTerminalFont;
    QFont defaultTerminalFont;
    QFont terminalFont;
    QString awsAccessKey;
    QString awsSecretKey;
    QString colorScheme;
};

#endif
