#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->newDialog = new NewDialog(this);
    QObject::connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    this->menuBar = new QMenuBar(this);

    QMenu *connMenu = new QMenu("Connections", menuBar);
    menuBar->addMenu(connMenu);
    connMenu->addAction("&New", this->newDialog, SLOT(open()));

    QMenu *helpMenu = new QMenu("Help", menuBar);
    menuBar->addMenu(helpMenu);
    helpMenu->addAction("About Qt", qApp, SLOT(aboutQt()));

    setMenuBar(menuBar);

    tabList = new QListWidget();
    tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    QObject::connect(tabList, SIGNAL (currentRowChanged(int)), this, SLOT (changeConnection(int)));

    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Session", this, SLOT(createNewSession()));
    tabStack = new QStackedWidget();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(tabStack);
    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(boxLayout);

    splitter->addWidget(tabList);
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
    tabs->setTabPosition(QTabWidget::North);
    tabs->addTab(console, QString("Session 1"));
    tabStack->addWidget(tabs);

    connEntry->tabs = tabs;
    this->sshConnByHost[label] = connEntry;

    tabList->addItem(label);
    tabList->setCurrentRow(tabList->count() - 1);
    //tabStack->setCurrentIndex(index);
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    console->setFocus();

    console->startShellProgram();
}

void MainWindow::createNewSession()
{
    int currentRow = tabList->currentRow();
    if (currentRow < 0) {
        return;
    }

    printf("currentRow: %d\n", currentRow);

    const QString usernameAndHost = tabList->item(currentRow)->text();
    std::cout << usernameAndHost.toUtf8().constData() << std::endl;
    SSHConnectionEntry *connEntry = this->sshConnByHost[usernameAndHost];
    QTabWidget *tabs = connEntry->tabs;
    QTermWidget *console = createNewTermWidget(connEntry->args);
    tabs->addTab(console, QString::asprintf("Session %d", tabs->count() + 1));
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    console->setFocus();

    console->startShellProgram();
}
