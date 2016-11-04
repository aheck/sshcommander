#include "MainWindow.h"

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
    connMenu->addAction("&Quit", qApp, SLOT(quit()));

    QMenu *helpMenu = new QMenu("Help", menuBar);
    menuBar->addMenu(helpMenu);
    helpMenu->addAction("About Qt", qApp, SLOT(aboutQt()));

    setMenuBar(menuBar);

    this->connectionModel = new SSHConnectionItemModel();
    this->tabList = new QListView();
    this->tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    this->tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    this->tabList->setModel(this->connectionModel);
    QObject::connect(this->tabList->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeConnection(QItemSelection, QItemSelection)));

    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Session", this, SLOT(createNewSession()));
    tabStack = new QStackedWidget();

    this->splitter = new QSplitter(Qt::Horizontal);
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(tabStack);
    QWidget *sshSessionsWidget = new QWidget();
    sshSessionsWidget->setLayout(boxLayout);

    QLabel *tabListLabel = new QLabel("SSH Hosts", this);
    QWidget *tabListWidget = new QWidget(this);
    QVBoxLayout *tabListLayout = new QVBoxLayout(tabListWidget);
    tabListLayout->addWidget(tabListLabel);
    tabListLayout->addWidget(this->tabList);
    tabListWidget->setLayout(tabListLayout);
    this->splitter->addWidget(tabListWidget);

    this->sessionInfoSplitter = new QSplitter(Qt::Vertical);

    this->sessionInfoSplitter->addWidget(sshSessionsWidget);

    CustomTabWidget *sshSessionsInfo = new CustomTabWidget();
    sshSessionsInfo->addTab(new QWidget(), "A");
    sshSessionsInfo->addTab(new QWidget(), "B");
    sshSessionsInfo->addTab(new QWidget(), "C");
    sshSessionsInfo->addTab(new QWidget(), "D");

    this->sessionInfoSplitter->addWidget(sshSessionsInfo);
    this->sessionInfoSplitter->setStretchFactor(0, 10);
    this->sessionInfoSplitter->setStretchFactor(1, 5);
    this->sessionInfoSplitter->setCollapsible(0, false);

    QTabWidget *rightWidget = new QTabWidget();
    rightWidget->addTab(sessionInfoSplitter, "SSH");

    this->awsWidget = new AWSWidget();
    rightWidget->addTab(this->awsWidget, "AWS");

    this->splitter->addWidget(rightWidget);
    this->splitter->setStretchFactor(0, 1);
    this->splitter->setStretchFactor(1, 15);
    this->splitter->setCollapsible(1, false);

    setCentralWidget(this->splitter);

    this->readSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::changeConnection(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();
    QModelIndex index = indexes.at(0);
    tabStack->setCurrentIndex(index.row());
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
    connEntry->name = label;

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

    CustomTabWidget *tabs = new CustomTabWidget();
    tabs->setTabsClosable(true);
    tabs->setTabPosition(CustomTabWidget::North);
    tabs->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    QObject::connect(tabs, SIGNAL (tabCloseRequested(int)), this, SLOT(closeSSHTab(int)));
    tabStack->addWidget(tabs);

    connEntry->tabs = tabs;
    this->sshConnByHost[label] = connEntry;

    this->connectionModel->appendConnectionEntry(connEntry);
    QModelIndex index = this->connectionModel->index(this->connectionModel->rowCount(QModelIndex()) - 1, 0, QModelIndex());
    this->tabList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
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

    CustomTabWidget *tabs = connEntry->tabs;

    QTermWidget *console = createNewTermWidget(connEntry->args);
    tabs->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    console->setFocus();

    console->startShellProgram();
}

const QString MainWindow::getCurrentUsernameAndHost()
{
    QModelIndexList indexes = this->tabList->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return QString("");
    }

    return this->connectionModel->data(indexes.first()).toString();
}

SSHConnectionEntry* MainWindow::getCurrentConnectionEntry()
{
    const QString usernameAndHost = this->getCurrentUsernameAndHost();

    return this->sshConnByHost[usernameAndHost];
}

CustomTabWidget* MainWindow::getCurrentTabWidget()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    return connEntry->tabs;
}

void MainWindow::closeSSHTab(int tabIndex)
{
    printf("tabIndex: %d\n", tabIndex);
    CustomTabWidget *tabWidget = this->getCurrentTabWidget();
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

void MainWindow::aboutToQuit()
{
    this->saveSettings();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    this->resize(settings.value("size", QSize(1000, 700)).toSize());
    this->move(settings.value("pos", QPoint(200, 200)).toPoint());
    this->splitter->restoreState(settings.value("splitterSizes").toByteArray());
    this->sessionInfoSplitter->restoreState(settings.value("sessionInfoSplitterSizes").toByteArray());
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.setValue("splitterSizes", this->splitter->saveState());
    settings.setValue("sessionInfoSplitterSizes", this->sessionInfoSplitter->saveState());
    settings.endGroup();
}
