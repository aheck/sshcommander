#include <iostream>

#include <QIcon>
#include <QStyle>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QListWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolBar>
#include <QPoint>
#include <QtDebug>

#include <qtermwidget.h>

#include "NewDialog.h"
#include "CustomTabWidget.h"
#include "AWSWidget.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

struct SSHConnectionEntry
{
public:
    SSHConnectionEntry();

    unsigned int nextSessionNumber;
    const QStringList *args;
    CustomTabWidget *tabs;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTermWidget* createNewTermWidget(const QStringList *args);

public slots:
    void changeConnection(int index);
    void createNewConnection();
    void createNewSession();
    void closeSSHTab(int tabIndex);
    void quitProgram();

private:
    const QString getCurrentUsernameAndHost();
    SSHConnectionEntry *getCurrentConnectionEntry();
    CustomTabWidget* getCurrentTabWidget();

    QMenuBar *menuBar;
    NewDialog *newDialog;
    QHash<QString, SSHConnectionEntry*> sshConnByHost;
    QListWidget *tabList;
    QStackedWidget *tabStack;
    QToolBar *toolBar;
    AWSWidget *awsWidget;
};

#endif
