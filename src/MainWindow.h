#include <QIcon>
#include <QStyle>
#include <QMainWindow>
#include <QMenuBar>
#include <QApplication>
#include <QListWidget>
#include <QStackedWidget>
#include <QToolBar>
#include <QPoint>
#include <QtDebug>

#include <qtermwidget.h>

#include "NewDialog.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

struct SSHConnectionEntry
{
    const QStringList *args;
    QTabWidget *tabs;
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

private:
    QMenuBar *menuBar;
    NewDialog *newDialog;
    QHash<QString, SSHConnectionEntry*> sshConnByHost;
    QListWidget *tabList;
    QStackedWidget *tabStack;
    QToolBar *toolBar;
};

#endif
