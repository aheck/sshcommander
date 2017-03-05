/*****************************************************************************
 *
 * TerminalViewWidget is the widget on the right-hand side of the main
 * window where the user can view and use the terminals of the current SSH
 * connection.
 *
 * It mainly consists of a toolbar for things like creating new SSH sessions
 * and a QStackedWidget which contains a TabbedTerminalWidget object for every
 * SSH connection created by the user. The TabbedTerminalWidget holds all
 * the SSH sessions of a connection. There is a second QStackedWidget
 * (widgetStack) which shows a DisabledWidget if there are no SSH connections
 * configured at the time.
 *
 ****************************************************************************/

#ifndef TERMINALVIEWWIDGET_H
#define TERMINALVIEWWIDGET_H

#include <memory>

#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolBar>
#include <QShortcut>
#include <QSplitter>
#include <QStackedWidget>

#include "AppletWidget.h"
#include "DisabledWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class TerminalViewWidget : public QWidget
{
    Q_OBJECT

public:
    TerminalViewWidget(Preferences &preferences, QWidget *parent = 0);

    void addConnection(Preferences &preferences, std::shared_ptr<SSHConnectionEntry> connEntry, TabbedTerminalWidget *tabs);
    void removeConnection(TabbedTerminalWidget *tabbedTerminal);
    void setCurrentConnection(int row);
    void setLastConnection();
    void setDisabledPageEnabled(bool enabled);
    void setFocusOnCurrentTerminal();
    void updateConsoleSettings(const QFont &font, const QString colorScheme);

signals:
    void requestToggleEnlarge();

public slots:
    void createNewSession();
    void restartCurrentSession();
    void toggleEnlarge();

    void prevTab();
    void nextTab();

private:
    bool enlarged;
    QAction *toggleEnlarged;
    QToolBar *toolBar;
    QStackedWidget *widgetStack;
    QStackedWidget *terminalStack;
    QStackedWidget *appletStack;
    QSplitter *terminalSplitter;
    QWidget *terminalPage;
};

#endif
