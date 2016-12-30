#ifndef TERMINALVIEWWIDGET_H
#define TERMINALVIEWWIDGET_H

#include <memory>

#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolBar>
#include <QShortcut>
#include <QStackedWidget>

#include "DisabledWidget.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

class TerminalViewWidget : public QWidget
{
    Q_OBJECT

public:
    TerminalViewWidget(QWidget *parent = 0);

    void addConnection(TabbedTerminalWidget *tabbedTerminal);
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
    QStackedWidget *connectionStack;
    QWidget *terminalPage;
};

#endif
