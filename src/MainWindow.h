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
#include <QStyle>
#include <QToolBar>
#include <QUrl>
#include <QtDebug>

#include <qtermwidget.h>

#include "AboutDialog.h"
#include "AWSInfoWidget.h"
#include "AWSWidget.h"
#include "CustomListView.h"
#include "CustomTabWidget.h"
#include "DisabledWidget.h"
#include "InactiveSessionWidget.h"
#include "MachineInfoWidget.h"
#include "NotesEditor.h"
#include "NewDialog.h"
#include "Preferences.h"
#include "PreferencesDialog.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

#include "globals.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTermWidget* createNewTermWidget(const QStringList *args, bool connectReceivedData);
    void readSettings();
    void saveSettings();

public slots:
    void changeConnection(const QItemSelection &selected, const QItemSelection &deselected);
    void createNewConnection();
    void createNewSession();
    void restartSession();
    void closeSSHTab(int tabIndex);
    void aboutToQuit();
    void createSSHConnectionToAWS(std::shared_ptr<AWSInstance> instance,
            std::vector<std::shared_ptr<AWSInstance>> vpcNeighbours, bool toPrivateIP);
    void showTabListContextMenu(QPoint pos);
    void removeConnection();
    void toggleSessionEnlarged();
    void openWebsite();
    void showPreferencesDialog();

private slots:
    void showNewDialog();
    void notesChanged();
    void nextTab();
    void prevTab();
    void editConnection();
    void dataReceived(const QString &text);

private:
    const QString getCurrentUsernameAndHost();
    std::shared_ptr<SSHConnectionEntry> getCurrentConnectionEntry();
    std::shared_ptr<SSHConnectionEntry> getConnectionEntryByTermWidget(QTermWidget *console);
    void removeTermWidgetMapping(QWidget *widget);
    CustomTabWidget* getCurrentTabWidget();
    QString findSSHKey(const QString keyname);
    void updateConnectionTabs();
    void selectFirstConnection();
    void selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry);
    void updateConsoleSettings(const QFont &font, const QString colorScheme);
    void setFocusOnCurrentTerminal();

    bool viewEnlarged;
    QAction *toggleEnlarged;
    AboutDialog *aboutDialog;
    QWidget *sshSessionsWidget;
    QWidget *hiddenPage;
    NewDialog *newDialog;
    NewDialog *editDialog;
    PreferencesDialog *preferencesDialog;
    QSplitter *splitter;
    QSplitter *sessionInfoSplitter;
    CustomListView *tabList;
    QStackedWidget *widgetStack;
    QStackedWidget *tabStack;
    QStackedWidget *sshSessionsStack;
    QToolBar *toolBar;
    AWSWidget *awsWidget;
    SSHConnectionItemModel *connectionModel;
    QTabWidget *rightWidget;
    MachineInfoWidget *machineInfo;
    AWSInfoWidget *awsInfo;
    QTabWidget *sshSessionsInfo;
    Preferences preferences;
    NotesEditor *notesEditor;
    std::map<QTermWidget *, std::shared_ptr<SSHConnectionEntry>> termToConn;
};

#endif
