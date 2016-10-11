#include "MainWindow.h"

#include <iostream>

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
    this->args = NULL;
    this->tabs = NULL;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->newDialog = new NewDialog(this);
    QObject::connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    this->menuBar = new QMenuBar(this);

    QMenu *connMenu = new QMenu("Connections", menuBar);
    menuBar->addMenu(connMenu);
    connMenu->addAction("&New", this->newDialog, SLOT(open()));
    connMenu->addSeparator();
    connMenu->addAction("&Quit", this, SLOT(quitProgram()));

    QMenu *helpMenu = new QMenu("Help", menuBar);
    menuBar->addMenu(helpMenu);
    helpMenu->addAction("About Qt", qApp, SLOT(aboutQt()));

    setMenuBar(menuBar);

    this->tabList = new QListWidget();
    this->tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    QObject::connect(this->tabList, SIGNAL (currentRowChanged(int)), this, SLOT (changeConnection(int)));

    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Session", this, SLOT(createNewSession()));
    tabStack = new QStackedWidget();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(tabStack);
    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(boxLayout);

    splitter->addWidget(this->tabList);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 15);
    splitter->setCollapsible(1, false);

    resize(1000, 700);
    setCentralWidget(splitter);
}

MainWindow::~MainWindow()
{
}

void MainWindow::changeConnection(int index)
{
    tabStack->setCurrentIndex(index);
}

QTermWidget* MainWindow::createNewTermWidget(const QStringList *args)
{
    const QString *program = new QString("/usr/bin/ssh");

    QFont font("monospace");
    font.setPointSize(12);
    font.setStyleHint(QFont::Monospace);
    font.setStyleHint(QFont::TypeWriter);
    font.setFamily("courier");

    QTermWidget *console = new QTermWidget(0);
    console->setShellProgram(*program);
    console->setArgs(*args);
    console->setTerminalFont(font);

    console->setColorScheme("COLOR_SCHEME_BLACK_ON_LIGHT_YELLOW");
    console->setScrollBarPosition(QTermWidget::ScrollBarRight);
    console->setKeyBindings("macbook");

    return console;
}

void MainWindow::createNewConnection()
{
    QString hostname = this->newDialog->hostnameLineEdit->text();
    QString username = this->newDialog->usernameLineEdit->text();
    //QString password = this->newDialog->passwordLineEdit->text();
    QString sshkey = this->newDialog->sshkeyLineEdit->text();
    QString label = QString("%1@%2").arg(username).arg(hostname);
    SSHConnectionEntry *connEntry = new SSHConnectionEntry();

    if (sshkey.isEmpty()) {
        const QStringList *args = new QStringList(label);
        connEntry->args = args;
    } else {
        QStringList *args = new QStringList(label);
        args->append("-i");
        args->append(sshkey);
        connEntry->args = args;
    }

    QTermWidget *console = createNewTermWidget(connEntry->args);

    // info output
    qDebug() << "* INFO *************************";
    qDebug() << " availableKeyBindings:" << console->availableKeyBindings();
    qDebug() << " keyBindings:" << console->keyBindings();
    qDebug() << " availableColorSchemes:" << console->availableColorSchemes();
    qDebug() << "* INFO END *********************";

    QTabWidget *tabs = new QTabWidget();
    tabs->setTabsClosable(true);
    tabs->setTabPosition(QTabWidget::North);
    tabs->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    QObject::connect(tabs, SIGNAL (tabCloseRequested(int)), this, SLOT(closeSSHTab(int)));
    tabStack->addWidget(tabs);

    connEntry->tabs = tabs;
    this->sshConnByHost[label] = connEntry;

    this->tabList->addItem(label);
    this->tabList->setCurrentRow(this->tabList->count() - 1);
    //tabStack->setCurrentIndex(index);
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    console->setFocus();

    console->startShellProgram();
}

void MainWindow::createNewSession()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    if (connEntry == NULL) {
        return;
    }

    QTabWidget *tabs = connEntry->tabs;

    QTermWidget *console = createNewTermWidget(connEntry->args);
    tabs->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    console->setFocus();

    console->startShellProgram();
}

const QString MainWindow::getCurrentUsernameAndHost()
{
    int currentRow = this->tabList->currentRow();
    if (currentRow < 0) {
        return QString("");
    }

    return this->tabList->item(currentRow)->text();
}

SSHConnectionEntry* MainWindow::getCurrentConnectionEntry()
{
    const QString usernameAndHost = this->getCurrentUsernameAndHost();

    return this->sshConnByHost[usernameAndHost];
}

QTabWidget* MainWindow::getCurrentTabWidget()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    return connEntry->tabs;
}

void MainWindow::closeSSHTab(int tabIndex)
{
    printf("tabIndex: %d\n", tabIndex);
    QTabWidget *tabWidget = this->getCurrentTabWidget();
    QTermWidget *termWidget = (QTermWidget*) tabWidget->widget(tabIndex);

    if (termWidget != NULL) {
        QMessageBox::StandardButton reply;
        const QString usernameAndHost = this->getCurrentUsernameAndHost();
        reply = QMessageBox::question(this, "Closing Session",
                QString("Do you really want to close SSH session '%1' with '%2'?").arg(tabWidget->tabText(tabIndex)).arg(usernameAndHost),
                QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        tabWidget->removeTab(tabIndex);
        delete termWidget;
    }
}

void MainWindow::quitProgram()
{
    qApp->quit();
}
