#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // dialogs
    this->aboutDialog = new AboutDialog();
    this->newDialog = new NewDialog();
    QObject::connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    this->viewEnlarged = false;

    // build the menu bar
    this->menuBar = new QMenuBar(this);

    QMenu *connMenu = new QMenu("Connections", menuBar);
    menuBar->addMenu(connMenu);
    connMenu->addAction("&New", this->newDialog, SLOT(exec()));
    connMenu->addSeparator();
    connMenu->addAction("&Quit", qApp, SLOT(quit()));

    QMenu *helpMenu = new QMenu("Help", menuBar);
    menuBar->addMenu(helpMenu);
    helpMenu->addAction("Website", this, SLOT(openWebsite()));
    helpMenu->addSeparator();
    helpMenu->addAction("About", this->aboutDialog, SLOT(exec()));

    setMenuBar(menuBar);

    this->machineInfo = new MachineInfoWidget();
    this->awsInfo = new AWSInfoWidget();

    this->widgetStack = new QStackedWidget();

    this->connectionModel = new SSHConnectionItemModel();
    this->tabList = new QListView();
    this->tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    this->tabList->setModel(this->connectionModel);
    QObject::connect(this->tabList->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeConnection(QItemSelection, QItemSelection)));
    this->tabList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->tabList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTabListContextMenu(QPoint)));

    this->splitter = new QSplitter(Qt::Horizontal);
    this->splitter->setContentsMargins(0, 0, 0, 0);

    // build sshSessionsWidget (the widget which contains the ssh sessions and
    // the ssh session tabs)
    this->sshSessionsStack = new QStackedWidget();
    QWidget *sshSessionsDisabledPage = new QWidget();
    sshSessionsDisabledPage->setLayout(new QVBoxLayout);
    QLabel *sshDisabledLabel = new QLabel("No SSH Connection");
    QFont font = sshDisabledLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    sshDisabledLabel->setFont(font);
    sshDisabledLabel->setStyleSheet("QLabel { color : grey; }");
    sshSessionsDisabledPage->layout()->setAlignment(Qt::AlignCenter);
    sshSessionsDisabledPage->layout()->addWidget(sshDisabledLabel);
    this->sshSessionsStack->addWidget(sshSessionsDisabledPage);
    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Session", this, SLOT(createNewSession()));
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_BrowserReload), "Restart Session", this, SLOT(restartSession()));
    toolBar->addAction(qApp->style()->standardIcon(QStyle::SP_ArrowUp), "Toggle Enlarged View", this, SLOT(toggleSessionEnlarged()));
    this->tabStack = new QStackedWidget();
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(this->tabStack);
    this->sshSessionsWidget = new QWidget();
    this->sshSessionsWidget->setLayout(boxLayout);
    this->sshSessionsStack->addWidget(this->sshSessionsWidget);

    QToolBar *connectionsToolbar = new QToolBar();
    connectionsToolbar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogNewFolder), "New Connection", this->newDialog, SLOT(exec()));
    connectionsToolbar->addAction(qApp->style()->standardIcon(QStyle::SP_TrashIcon), "Delete Connection", this, SLOT(removeConnection()));
    QWidget *tabListWidget = new QWidget(this);
    QVBoxLayout *tabListLayout = new QVBoxLayout(tabListWidget);
    tabListLayout->addWidget(connectionsToolbar);
    tabListLayout->addWidget(this->tabList);
    tabListWidget->setLayout(tabListLayout);
    this->splitter->addWidget(tabListWidget);

    this->sessionInfoSplitter = new QSplitter(Qt::Vertical);

    this->sessionInfoSplitter->addWidget(this->sshSessionsStack);

    this->sshSessionsInfo = new QTabWidget();
    this->sshSessionsInfo->addTab(this->machineInfo, "Machine");
    this->sshSessionsInfo->addTab(this->awsInfo, "AWS");

    this->sessionInfoSplitter->addWidget(this->sshSessionsInfo);
    this->sessionInfoSplitter->setStretchFactor(0, 10);
    this->sessionInfoSplitter->setStretchFactor(1, 5);
    this->sessionInfoSplitter->setCollapsible(0, false);

    this->rightWidget = new QTabWidget();
    rightWidget->addTab(this->sessionInfoSplitter, "SSH");

    this->awsWidget = new AWSWidget();
    QObject::connect(this->awsWidget, SIGNAL(newConnection(AWSInstance)), this, SLOT(createSSHConnectionToAWS(AWSInstance)));
    rightWidget->addTab(this->awsWidget, "AWS");

    this->splitter->addWidget(rightWidget);
    this->splitter->setStretchFactor(0, 1);
    this->splitter->setStretchFactor(1, 15);
    this->splitter->setCollapsible(1, false);

    this->widgetStack->addWidget(this->splitter);
    this->hiddenPage = new QWidget();
    QVBoxLayout *hiddenLayout = new QVBoxLayout();
    hiddenLayout->setContentsMargins(0, 0, 0, 0);
    this->hiddenPage->setLayout(hiddenLayout);
    this->widgetStack->addWidget(this->hiddenPage);
    this->widgetStack->setCurrentIndex(0);

    setCentralWidget(this->widgetStack);

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
    int port = this->newDialog->getPortNumber();
    QString userAtHost = QString("%1@%2").arg(username).arg(hostname);
    SSHConnectionEntry *connEntry;

    // Check if a connection for username@hostname already exists.
    // If this is the case we create no new connection but bring the existing
    // connection to the foreground.
    connEntry = this->connectionModel->getConnEntryByName(userAtHost);
    if (connEntry != nullptr) {
        this->rightWidget->setCurrentIndex(0);
        this->selectConnection(connEntry);
        return;
    }

    connEntry = new SSHConnectionEntry();
    connEntry->name = userAtHost;
    connEntry->hostname = hostname;
    connEntry->username = username;

    if (this->newDialog->isAwsInstance) {
        connEntry->isAwsInstance = true;
        connEntry->awsInstance = this->newDialog->awsInstance;
    }

    // build the argument list for ssh
    connEntry->args->clear();
    if (!sshkey.isEmpty()) {
        connEntry->args->append("-i");
        connEntry->args->append(sshkey);
    }

    if (port != QString(DEFAULT_SSH_PORT).toInt(nullptr, 10)) {
        connEntry->args->append("-p");
        connEntry->args->append(QString(port));
    }

    connEntry->args->append(userAtHost);

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
    this->sshSessionsStack->setCurrentIndex(1);
    this->rightWidget->setCurrentIndex(0);
    console->setFocus();

    console->startShellProgram();
}

void MainWindow::createNewSession()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    if (connEntry == nullptr) {
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
    QWidget *oldWidget = nullptr;
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();
    if (connEntry == nullptr) {
        return;
    }

    CustomTabWidget *tabs = connEntry->tabs;

    if (tabs->count() == 0) {
        return;
    }

    int tabIndex = tabs->currentIndex();
    const QString tabText = tabs->tabText(tabIndex);

    if (tabs->currentWidget() != nullptr) {
        if (QString("QTermWidget") == tabs->currentWidget()->metaObject()->className()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Restart Session?",
                    QString("Do you really want to restart SSH session '%1' with '%2'?").arg(tabText).arg(connEntry->name),
                    QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            }
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

    if (termWidget != nullptr) {
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

    QString filename = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("connections.json");
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        std::cout << "Failed to open connections.json" << std::endl;
        return;
    }

    QByteArray fileContent(file.readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContent);

    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray connArray(jsonObj["connections"].toArray());
    for (QJsonValue curValue : connArray) {
        SSHConnectionEntry *entry = new SSHConnectionEntry();
        entry->read(curValue.toObject());

        CustomTabWidget *tabs = new CustomTabWidget();
        tabs->setTabsClosable(true);
        tabs->setTabPosition(CustomTabWidget::North);

        for (int i = 0; i < entry->tabNames->size(); i++) {
            InactiveSessionWidget *inactiveSessionWidget = new InactiveSessionWidget();
            QObject::connect(inactiveSessionWidget, SIGNAL(createSession()), this, SLOT(restartSession()));
            tabs->addTab(inactiveSessionWidget, entry->tabNames->at(i));
        }

        QObject::connect(tabs, SIGNAL (tabCloseRequested(int)), this, SLOT(closeSSHTab(int)));
        tabStack->addWidget(tabs);

        entry->tabs = tabs;

        this->connectionModel->appendConnectionEntry(entry);

        this->sshSessionsStack->setCurrentIndex(1);
        this->rightWidget->setCurrentIndex(0);
        this->selectFirstConnection();
    }

    file.close();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    // We store all our program settings with QSettings.
    settings.beginGroup("MainWindow");
    settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.setValue("splitterSizes", this->splitter->saveState());
    settings.setValue("sessionInfoSplitterSizes", this->sessionInfoSplitter->saveState());
    settings.setValue("sessionInfoSplitterSizes", this->sessionInfoSplitter->saveState());
    settings.setValue("selectedAwsRegion", this->awsWidget->getRegion());
    settings.endGroup();

    // The SSH connections are serialized to JSON and stored in a separate file.
    // This saves us from having to deal with limitations of the native settings
    // storage mechanism used by QSettings.
    QJsonObject jsonObject;
    QJsonDocument jsonDoc;

    QJsonArray connArray = QJsonArray();

    for (int i = 0; i < this->connectionModel->rowCount(QModelIndex()); i++) {
        QJsonObject curObj;
        SSHConnectionEntry *entry = this->connectionModel->getConnEntry(i);

        entry->tabNames->clear();
        for (int j = 0; j < entry->tabs->count(); j++) {
            entry->tabNames->append(entry->tabs->tabText(j));
        }

        entry->write(curObj);
        connArray.append(curObj);
    }

    jsonObject["connections"] = connArray;
    jsonDoc.setObject(jsonObject);

    QDir jsonDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (!jsonDir.exists()) {
        if (!jsonDir.mkpath(jsonDir.path())) {
            QMessageBox msgBox;
            msgBox.setText("Failed to create directory '" + jsonDir.path() + "'");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            return;
        }
    }

    QString jsonFilePath = jsonDir.filePath("connections.json");
    QFile file(jsonFilePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Failed to open file '" + jsonFilePath + "' for writing.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    qint64 bytesWritten = file.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        QMessageBox msgBox;
        msgBox.setText("Failed to write to file '" + jsonFilePath + "'");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    file.close();
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

    if (entry == nullptr) {
        return;
    }

    QMessageBox::StandardButton reply;
    const QString usernameAndHost = this->getCurrentUsernameAndHost();
    reply = QMessageBox::question(this, "Removing Connection",
            QString("Do you really want to remove the SSH connection '%1'?").arg(usernameAndHost),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    CustomTabWidget *tabWidget = entry->tabs;
    for(int i = tabWidget->count(); i >= 0; --i) {
        QTermWidget *termWidget = (QTermWidget*) tabWidget->widget(i);
        tabWidget->removeTab(i);
        delete termWidget;
    }

    this->connectionModel->removeConnectionEntry(entry);
    this->tabStack->removeWidget(tabWidget);

    if (this->connectionModel->rowCount(QModelIndex()) == 0) {
        this->machineInfo->setMachineEnabled(false);
        this->awsInfo->setAWSEnabled(false);
        this->sshSessionsStack->setCurrentIndex(0);
    }

    delete entry;
}

void MainWindow::updateConnectionTabs()
{
    SSHConnectionEntry *connEntry = this->getCurrentConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    this->machineInfo->setMachineEnabled(true);
    this->machineInfo->setHostname(connEntry->hostname);
    this->machineInfo->setUsername(connEntry->username);

    if (connEntry->isAwsInstance) {
        this->awsInfo->setAWSEnabled(true);
        this->awsInfo->update(connEntry->awsInstance);
    } else {
        this->awsInfo->setAWSEnabled(false);
    }
}

void MainWindow::toggleSessionEnlarged()
{
    if (this->getCurrentConnectionEntry() == nullptr) {
        return;
    }

    if (this->viewEnlarged) {
        this->hiddenPage->layout()->removeWidget(this->sshSessionsStack);
        this->sessionInfoSplitter->insertWidget(0, this->sshSessionsStack);
        this->widgetStack->setCurrentIndex(0);
        this->menuBar->show();
        this->viewEnlarged = false;
    } else {
        this->sshSessionsStack->setParent(this->hiddenPage);
        this->hiddenPage->layout()->addWidget(this->sshSessionsStack);
        this->widgetStack->setCurrentIndex(1);
        this->menuBar->hide();
        this->viewEnlarged = true;
    }
}

void MainWindow::openWebsite()
{
    QDesktopServices::openUrl(QUrl("https://github.com/aheck/sshcommander"));
}

void MainWindow::selectFirstConnection()
{
    if (this->connectionModel->rowCount(QModelIndex()) < 1) {
        return;
    }

    SSHConnectionEntry *entry = this->connectionModel->getConnEntry(0);
    this->selectConnection(entry);
}

void MainWindow::selectConnection(SSHConnectionEntry *connEntry)
{
    QModelIndex index = this->connectionModel->getIndexForSSHConnectionEntry(connEntry);
    this->tabList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}
