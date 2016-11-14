#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->newDialog = new NewDialog(this);
    QObject::connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    this->menuBar = new QMenuBar(this);

    QMenu *connMenu = new QMenu("Connections", menuBar);
    menuBar->addMenu(connMenu);
    connMenu->addAction("&New", this->newDialog, SLOT(exec()));
    connMenu->addSeparator();
    connMenu->addAction("&Quit", qApp, SLOT(quit()));

    QMenu *helpMenu = new QMenu("Help", menuBar);
    menuBar->addMenu(helpMenu);
    helpMenu->addAction("About Qt", qApp, SLOT(aboutQt()));

    setMenuBar(menuBar);

    this->connectionModel = new SSHConnectionItemModel();
    this->tabList = new QListView();
    this->tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    this->tabList->setModel(this->connectionModel);
    QObject::connect(this->tabList->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeConnection(QItemSelection, QItemSelection)));
    this->tabList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->tabList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTabListContextMenu(QPoint)));

    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Session", this, SLOT(createNewSession()));
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_BrowserReload), "Restart Session", this, SLOT(restartSession()));
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

    this->sshSessionsInfo = new QTabWidget();
    sshSessionsInfo->addTab(&this->machineInfo, "Machine");
    sshSessionsInfo->addTab(&this->awsInfo, "AWS");

    this->sessionInfoSplitter->addWidget(sshSessionsInfo);
    this->sessionInfoSplitter->setStretchFactor(0, 10);
    this->sessionInfoSplitter->setStretchFactor(1, 5);
    this->sessionInfoSplitter->setCollapsible(0, false);

    this->rightWidget = new QTabWidget();
    rightWidget->addTab(sessionInfoSplitter, "SSH");

    this->awsWidget = new AWSWidget();
    QObject::connect(this->awsWidget, SIGNAL(newConnection(AWSInstance)), this, SLOT(createSSHConnectionToAWS(AWSInstance)));
    rightWidget->addTab(this->awsWidget, "AWS");

    this->splitter->addWidget(rightWidget);
    this->splitter->setStretchFactor(0, 1);
    this->splitter->setStretchFactor(1, 15);
    this->splitter->setCollapsible(1, false);

    setCentralWidget(this->splitter);

    this->readSettings();

    this->awsWidget->loadInstances();
}

MainWindow::~MainWindow()
{
}

void MainWindow::changeConnection(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();

    if (indexes.size() == 0) {
        return;
    }

    QModelIndex index = indexes.at(0);
    tabStack->setCurrentIndex(index.row());
    this->updateConnectionTabs();
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
    SSHConnectionEntry *connEntry;

    // Check if a connection for username@hostname already exists.
    // If this is the case we create no new connection but bring the existing
    // connection to the foreground.
    connEntry = this->connectionModel->getConnEntryByName(label);
    if (connEntry != NULL) {
        this->rightWidget->setCurrentIndex(0);
        QModelIndex index = this->connectionModel->getIndexForSSHConnectionEntry(connEntry);
        this->tabList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        return;
    }

    connEntry = new SSHConnectionEntry();
    connEntry->name = label;
    connEntry->hostname = hostname;
    connEntry->username = username;

    if (this->newDialog->isAwsInstance) {
        connEntry->isAwsInstance = true;
        connEntry->awsInstance = this->newDialog->awsInstance;
    }

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

    this->connectionModel->appendConnectionEntry(connEntry);
    QModelIndex index = this->connectionModel->index(this->connectionModel->rowCount(QModelIndex()) - 1, 0, QModelIndex());
    this->tabList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    this->rightWidget->setCurrentIndex(0);
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

    console->startShellProgram();

    tabs->setFocus();
    console->setFocus();
}

void MainWindow::restartSession()
{
    QWidget *oldWidget = NULL;
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    if (connEntry == NULL) {
        return;
    }

    CustomTabWidget *tabs = connEntry->tabs;

    if (tabs->count() == 0) {
        return;
    }

    int tabIndex = tabs->currentIndex();
    const QString tabText = tabs->tabText(tabIndex);

    if (tabs->currentWidget() != NULL) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Restart Session?",
                QString("Do you really want to restart SSH session '%1' with '%2'?").arg(tabText).arg(connEntry->name),
                QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        oldWidget = tabs->currentWidget();
    }

    QTermWidget *console = createNewTermWidget(connEntry->args);
    tabs->setUpdatesEnabled(false);
    tabs->removeTab(tabIndex);
    tabs->insertTab(tabIndex, console, tabText);
    tabs->setCurrentIndex(tabIndex);
    tabs->setUpdatesEnabled(true);

    console->startShellProgram();

    tabs->setFocus();
    console->setFocus();

    if (oldWidget) {
        delete oldWidget;
    }
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

    return this->connectionModel->getConnEntryByName(usernameAndHost);
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
    this->awsWidget->setRegion(settings.value("selectedAwsRegion", "").toString());
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
    settings.setValue("sessionInfoSplitterSizes", this->sessionInfoSplitter->saveState());
    settings.setValue("selectedAwsRegion", this->awsWidget->getRegion());
    settings.endGroup();
}

void MainWindow::createSSHConnectionToAWS(const AWSInstance &instance)
{
    this->newDialog->hostnameLineEdit->setText(instance.publicIP);
    this->newDialog->sshkeyLineEdit->setText(this->findSSHKey(instance.keyname));
    this->newDialog->usernameLineEdit->setFocus();

    this->newDialog->isAwsInstance = true;
    this->newDialog->awsInstance = instance;
    this->newDialog->exec();
    this->newDialog->isAwsInstance = false;
}

QString MainWindow::findSSHKey(const QString keyname)
{
    QString result;

    QString keynameInDotSSH = QDir::homePath() + "/.ssh/" + keyname;
    if (QFileInfo::exists(keynameInDotSSH)) {
        QFileInfo info(keynameInDotSSH);

        if (info.isFile()) {
            result = keynameInDotSSH;
        }
    }

    return result;
}

void MainWindow::showTabListContextMenu(QPoint pos)
{
    QPoint globalPos = this->tabList->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Delete", this, SLOT(removeConnection()));

    if (this->tabList->indexAt(pos).isValid()) {
        menu.exec(globalPos);
    }
}

void MainWindow::removeConnection()
{
    SSHConnectionEntry *entry = this->getCurrentConnectionEntry();

    CustomTabWidget *tabWidget = entry->tabs;
    for(int i = tabWidget->count(); i >= 0; --i) {
        QTermWidget *termWidget = (QTermWidget*) tabWidget->widget(i);
        tabWidget->removeTab(i);
        delete termWidget;
    }

    this->connectionModel->removeConnectionEntry(entry);
    this->tabStack->removeWidget(tabWidget);

    delete entry;
}

void MainWindow::updateConnectionTabs()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();

    if (connEntry == NULL) {
        return;
    }

    this->machineInfo.setHostname(connEntry->hostname);
    this->machineInfo.setUsername(connEntry->username);

    if (connEntry->isAwsInstance) {
        this->awsInfo.setInstanceId(connEntry->awsInstance.id);
        this->awsInfo.setRegion(connEntry->awsInstance.region);
    }
}
