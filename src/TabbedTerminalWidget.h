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
    explicit TabbedTerminalWidget(Preferences *preferences, std::weak_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);

    void addTerminalSession();
    void addInactiveSession(const QString title);

public slots:
    void restartCurrentSession();

private slots:
    void dataReceived(const QString &text);
    void closeTab(int tabIndex);

private:
    Preferences *preferences;
    std::weak_ptr<SSHConnectionEntry> connEntryWeak;
    QTermWidget* createNewTermWidget(const QStringList *args, bool connectReceivedData);
};

#endif
