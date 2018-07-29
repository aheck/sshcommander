/*****************************************************************************
 *
 * MainWindow implements the main window of the UI which holds everything
 * together.
 *
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>

#include <QApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QListView>
#include <QPoint>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBar>
#include <QUrl>
#include <QWindow>
#include <QtDebug>

#include <qtermwidget.h>

#include "AboutDialog.h"
#include "AWSConsoleWindow.h"
#include "AWSWidget.h"
#include "ConnectionListWidget.h"
#include "DisabledWidget.h"
#include "NewDialog.h"
#include "Preferences.h"
#include "PreferencesDialog.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"
#include "TabbedTerminalWidget.h"
#include "TerminalViewWidget.h"

#include "globals.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings();
    void saveSettings();
    bool askToQuit();

public slots:
    void changeConnection(int row);
    void moveConnection(int originRow, int targetRow);
    void createNewConnection();
    void quit();
    void closeEvent(QCloseEvent *event) override;
    void aboutToQuit();
    void createSSHConnectionToAWS(std::shared_ptr<AWSInstance> instance,
            std::vector<std::shared_ptr<AWSInstance>> vpcNeighbours, bool toPrivateIP);
    void openWebsite();
    void showPreferencesDialog();

    void showNewDialog();
    void connectionRemoved(std::shared_ptr<SSHConnectionEntry> connEntry);
    void toggleEnlargeWidget();

private slots:
    void toggleAwsConsole(bool show);
    void toggleDetachAwsConsole(bool detach);

private:
    void removeTermWidgetMapping(QWidget *widget);
    QString findSSHKey(const QString keyname);
    void updateConnectionTabs();
    void selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry);

    bool viewEnlarged;
    bool awsConsoleDetached;

    QWidget *hiddenPage;
    QStackedWidget *widgetStack;
    QToolBar *toolBar;

    QSplitter *splitter;

    QStackedWidget *rightWidget;
    QTabWidget *appletTab;

    // Models
    SSHConnectionItemModel *connectionModel;

    // Dialogs
    AboutDialog *aboutDialog;
    NewDialog *newDialog;
    PreferencesDialog *preferencesDialog;

    // Main widgets
    AWSWidget *awsWidget;
    ConnectionListWidget *connectionList;
    TerminalViewWidget *terminalView;

    // this window is used for the AWS console when the user detaches it from
    // the main window
    AWSConsoleWindow *awsConsoleWindow;

    // we remove terminalView from and add it again to this container when
    // toggling enlarged view
    QWidget *terminalViewContainer;
};

#endif
