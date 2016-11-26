#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QColor>
#include <QObject>
#include <QString>
#include <QFont>

class Preferences : public QObject
{
    Q_OBJECT

public:
    Preferences();

    QFont getTerminalFont();
    void setTerminalFont(const QFont &font);

private:
    QFont terminalFont;
};

#endif
