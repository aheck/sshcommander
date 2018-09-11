/*****************************************************************************
 *
 * SSHTermWidget is a specialization of QTermWidget which provides automatic
 * password authentication.
 *
 ****************************************************************************/

#ifndef SSHTERMWIDGET_H
#define SSHTERMWIDGET_H

#include <iostream>
#include <memory>

#include "qtermwidget.h"

#include "SSHConnectionEntry.h"

class SSHConnectionEntry;

class SSHTermWidget : public QTermWidget
{
    Q_OBJECT

public:
    SSHTermWidget(const QStringList *args, std::weak_ptr<SSHConnectionEntry> connEntryWeak, QWidget *parent = 0);

public slots:
    void dataReceived(const QString &text);
    void updateConsoleSettings(const QFont &font, const QString &colorScheme);

private:
    int passwordLineCounter;
    std::weak_ptr<SSHConnectionEntry> connEntryWeak;
    QRegExp passwordRegex1;
    QRegExp passwordRegex2;
};

#endif
