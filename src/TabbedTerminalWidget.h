/*****************************************************************************
 *
 * TabbedTerminalWidget is the widget that organizes multiple QTermWidget
 * objects in tabs.
 *
 * It also handles expect-like automatization of SSH sessions like entering a
 * password in case the user has chosen password-based authentication.
 *
 ****************************************************************************/

#ifndef TABBEDTERMINALWIDGET_H
#define TABBEDTERMINALWIDGET_H

#include <QMessageBox>
#include <QTabWidget>

#include "qtermwidget.h"

#include "CustomTabBar.h"
#include "InactiveSessionWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"

struct SSHConnectionEntry;

class TabbedTerminalWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabbedTerminalWidget(std::weak_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);
    ~TabbedTerminalWidget();

    void addTerminalSession();
    void addInactiveSession(const QString title);

public slots:
    void restartCurrentSession();

private slots:
    void dataReceived(const QString &text);
    void closeTab(int tabIndex);

private:
    std::weak_ptr<SSHConnectionEntry> connEntryWeak;
    std::map<QTermWidget*, int> passwordLineCounter;

    QTermWidget* createNewTermWidget(const QStringList *args, bool connectReceivedData);
};

#endif
