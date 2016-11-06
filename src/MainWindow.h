#include <iostream>

#include <QFileInfo>
#include <QIcon>
#include <QStyle>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QListView>
#include <QSettings>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBar>
#include <QPoint>
#include <QtDebug>

#include <qtermwidget.h>

#include "AWSWidget.h"
#include "CustomTabWidget.h"
#include "NewDialog.h"
#include "SSHConnectionEntry.h"
#include "SSHConnectionItemModel.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTermWidget* createNewTermWidget(const QStringList *args);
    void readSettings();
    void saveSettings();

public slots:
    void changeConnection(const QItemSelection &selected, const QItemSelection &deselected);
    void createNewConnection();
    void createNewSession();
    void closeSSHTab(int tabIndex);
    void aboutToQuit();
    void createSSHConnectionToAWS(const AWSInstance &instance);

private:
    const QString getCurrentUsernameAndHost();
    SSHConnectionEntry *getCurrentConnectionEntry();
    CustomTabWidget* getCurrentTabWidget();
    QString findSSHKey(const QString keyname);

    QMenuBar *menuBar;
    NewDialog *newDialog;
    QSplitter *splitter;
    QSplitter *sessionInfoSplitter;
    QListView *tabList;
    QStackedWidget *tabStack;
    QToolBar *toolBar;
    AWSWidget *awsWidget;
    SSHConnectionItemModel *connectionModel;
    QTabWidget *rightWidget;
};

#endif
