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

#include <memory>

#include <QMessageBox>
#include <QMouseEvent>
#include <QTabWidget>

#include "qtermwidget.h"

#include "CustomTabBar.h"
#include "DetachedSessionWidget.h"
#include "DetachedTerminalWindow.h"
#include "InactiveSessionWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"
#include "TerminalContainer.h"

struct SSHConnectionEntry;

struct TerminalSessionEntry {
    QUuid uuid;
    QString sessionName;
    bool detached;
    TerminalContainer *container;
    DetachedTerminalWindow *window;

    TerminalSessionEntry();
};

class TabbedTerminalWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabbedTerminalWidget(std::weak_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);
    ~TabbedTerminalWidget();

    void addTerminalSession();
    void addInactiveSession(const QString title);
    void closeAllDetachedWindows();

public slots:
    void startInactiveSession(QUuid uuid);
    void restartCurrentSession();
    void detachTab(int index);
    void reattachTab(QUuid uuid);
    void showDetachedWindow(QUuid uuid);

private slots:
    void dataReceived(const QString &text);
    void closeTab(int tabIndex);

private:
    QTermWidget* createNewTermWidget(const QStringList *args, bool connectReceivedData);

    std::weak_ptr<SSHConnectionEntry> connEntryWeak;
    std::map<QUuid, TerminalSessionEntry*> terminalSessions;
    std::map<QTermWidget*, int> passwordLineCounter;

signals:
    void terminalAttachmentChanged();
};

#endif
