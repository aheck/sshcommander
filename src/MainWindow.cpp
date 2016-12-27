#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // read in the user preferences from QSettings
    this->preferences.read();

    // dialogs
    this->aboutDialog = new AboutDialog();
    this->newDialog = new NewDialog();
    this->editDialog = new NewDialog(true);
    this->preferencesDialog = new PreferencesDialog();
    QObject::connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    this->viewEnlarged = false;

    // build the menu bar
    QMenuBar *menuBar = new QMenuBar(0);

    QMenu *connMenu = new QMenu(tr("Connection"));
    QAction *newRole = connMenu->addAction(tr("&New"), this, SLOT(showNewDialog()));
    newRole->setMenuRole(QAction::NoRole);
    connMenu->addSeparator();
    QAction *quitRole = connMenu->addAction(tr("&Quit"), qApp, SLOT(quit()));
    quitRole->setMenuRole(QAction::QuitRole);

    QMenu *editMenu = new QMenu(tr("Edit"));
    QAction *preferencesAction = editMenu->addAction(tr("Preferences"), this, SLOT(showPreferencesDialog()));
    preferencesAction->setMenuRole(QAction::PreferencesRole);

    QMenu *helpMenu = new QMenu(tr("Help"));
    QAction *websiteAction = helpMenu->addAction(tr("Website"), this, SLOT(openWebsite()));
    websiteAction->setMenuRole(QAction::NoRole);
    helpMenu->addSeparator();
    QAction *aboutAction = helpMenu->addAction(tr("About"), this->aboutDialog, SLOT(exec()));
    aboutAction->setMenuRole(QAction::ApplicationSpecificRole);

    menuBar->addMenu(connMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(helpMenu);

    this->setMenuBar(menuBar);

    // create the connection applets
    this->machineInfo = new MachineInfoWidget();
    this->awsInfo = new AWSInfoWidget(&this->preferences);

    this->widgetStack = new QStackedWidget();

    this->splitter = new QSplitter(Qt::Horizontal);
    this->splitter->setContentsMargins(0, 0, 0, 0);

    // build sshSessionsWidget (the widget which contains the ssh sessions and
    // the ssh session tabs)
    this->sshSessionsStack = new QStackedWidget();

    DisabledWidget *disabledWidget = new DisabledWidget("No SSH Connection");
    this->sshSessionsStack->addWidget(disabledWidget);

    toolBar = new QToolBar("toolBar", 0);
    toolBar->addAction(QIcon(":/images/utilities-terminal"), "New Session", this, SLOT(createNewSession()));
    toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Restart Session", this, SLOT(restartSession()));
    this->toggleEnlarged = toolBar->addAction(QIcon(":/images/view-fullscreen.svg"),
            "Toggle Enlarged View (F10)", this, SLOT(toggleSessionEnlarged()));
    this->toggleEnlarged->setCheckable(true);
    this->tabStack = new QStackedWidget();
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(this->tabStack);
    this->sshSessionsWidget = new QWidget();
    this->sshSessionsWidget->setLayout(boxLayout);
    this->sshSessionsStack->addWidget(this->sshSessionsWidget);

    // create the connection list and its model
    this->connectionModel = new SSHConnectionItemModel();
    this->connectionList = new ConnectionListWidget(this->connectionModel);

    connect(this->connectionList, SIGNAL(showNewDialog()), this, SLOT(showNewDialog()));
    connect(this->connectionList, SIGNAL(editConnection()), this, SLOT(editConnection()));
    connect(this->connectionList, SIGNAL(removeConnection()), this, SLOT(removeConnection()));
    connect(this->connectionList, SIGNAL(changeConnection(QItemSelection, QItemSelection)),
            this, SLOT(changeConnection(QItemSelection, QItemSelection)));

    this->splitter->addWidget(this->connectionList);

    this->sessionInfoSplitter = new QSplitter(Qt::Vertical);

    this->sessionInfoSplitter->addWidget(this->sshSessionsStack);

    this->notesEditor = new NotesEditor();
    QObject::connect(this->notesEditor, SIGNAL(textChanged()), this, SLOT(notesChanged()));

    this->sshSessionsInfo = new QTabWidget();
    this->sshSessionsInfo->addTab(this->machineInfo, "Machine");
    this->sshSessionsInfo->addTab(this->notesEditor, "Notes");
    this->sshSessionsInfo->addTab(this->awsInfo, "AWS");

    this->sessionInfoSplitter->addWidget(this->sshSessionsInfo);
    this->sessionInfoSplitter->setStretchFactor(0, 10);
    this->sessionInfoSplitter->setStretchFactor(1, 5);
    this->sessionInfoSplitter->setCollapsible(0, false);

    this->rightWidget = new QTabWidget();
    rightWidget->addTab(this->sessionInfoSplitter, "SSH");

    this->awsWidget = new AWSWidget(&this->preferences);
    QObject::connect(this->awsWidget, SIGNAL(newConnection(std::shared_ptr<AWSInstance>,
            std::vector<std::shared_ptr<AWSInstance>>, bool)), this,
            SLOT(createSSHConnectionToAWS(std::shared_ptr<AWSInstance>,
            std::vector<std::shared_ptr<AWSInstance>>, bool)));
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

    // assign shortcuts
    QShortcut *toggleEnlargedShortcut = new QShortcut(QKeySequence(Qt::Key_F10), this);
    toggleEnlargedShortcut->setContext(Qt::ApplicationShortcut);
    QObject::connect(toggleEnlargedShortcut, SIGNAL(activated()), this, SLOT(toggleSessionEnlarged()));

    QShortcut *nextTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this);
    nextTabShortcut->setContext(Qt::ApplicationShortcut);
    QObject::connect(nextTabShortcut, SIGNAL(activated()), this, SLOT(nextTab()));

    QShortcut *prevTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this);
    prevTabShortcut->setContext(Qt::ApplicationShortcut);
    QObject::connect(prevTabShortcut, SIGNAL(activated()), this, SLOT(prevTab()));

    setCentralWidget(this->widgetStack);

    this->readSettings();

    this->awsWidget->loadInstances();
}

MainWindow::~MainWindow()
{
}

void MainWindow::showNewDialog()
{
    this->newDialog->clear();
    this->newDialog->updateSSHKeys();
    this->newDialog->exec();
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

    this->setFocusOnCurrentTerminal();
}

QTermWidget* MainWindow::createNewTermWidget(const QStringList *args, bool connectReceivedData)
{
    const QString *program = new QString("/usr/bin/ssh");

    QTermWidget *console = new QTermWidget(0);

    if (connectReceivedData) {
        QObject::connect(console, SIGNAL(receivedData(QString)), this, SLOT(dataReceived(QString)));
    }

    console->setAutoClose(false);
    console->setShellProgram(*program);
    console->setArgs(*args);
    console->setTerminalFont(this->preferences.getTerminalFont());

    console->setColorScheme(this->preferences.getColorScheme());
    console->setScrollBarPosition(QTermWidget::ScrollBarRight);

    return console;
}

void MainWindow::createNewConnection()
{
    QString hostname = this->newDialog->getHostname();
    QString username = this->newDialog->getUsername();
    QString password = this->newDialog->getPassword();
    QString userAtHost = QString("%1@%2").arg(username).arg(hostname);
    std::shared_ptr<SSHConnectionEntry> connEntry;

    // Check if a connection for username@hostname already exists.
    // If this is the case we create no new connection but bring the existing
    // connection to the foreground.
    connEntry = this->connectionModel->getConnEntryByName(userAtHost);
    if (connEntry != nullptr) {
        this->rightWidget->setCurrentIndex(0);
        this->connectionList->selectConnection(connEntry);
        return;
    }

    connEntry = std::make_shared<SSHConnectionEntry>();
    connEntry->name = userAtHost;
    connEntry->sshkey = this->newDialog->getSSHKey();
    connEntry->port = this->newDialog->getPortNumber();
    connEntry->hostname = hostname;
    connEntry->username = username;
    connEntry->password = password;
    connEntry->shortDescription = this->newDialog->getShortDescription();
    connEntry->hopHosts = this->newDialog->getHopHosts();

    if (this->newDialog->isAwsInstance) {
        connEntry->isAwsInstance = true;
        connEntry->awsInstance = this->newDialog->awsInstance;
    }

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    this->termToConn[console] = connEntry;

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
    this->connectionList->selectLast();
    tabs->setCurrentWidget(console);
    tabs->setFocus();
    this->sshSessionsStack->setCurrentIndex(1);
    this->rightWidget->setCurrentIndex(0);
    console->setFocus();

    console->startShellProgram();
}

void MainWindow::createNewSession()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();
    if (connEntry == nullptr) {
        return;
    }

    CustomTabWidget *tabs = connEntry->tabs;

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    this->termToConn[console] = connEntry;
    tabs->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    tabs->setCurrentWidget(console);

    console->startShellProgram();

    tabs->setFocus();
    console->setFocus();
}

void MainWindow::restartSession()
{
    QWidget *oldWidget = nullptr;
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();
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

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    this->termToConn[console] = connEntry;
    tabs->setUpdatesEnabled(false);
    tabs->removeTab(tabIndex);
    tabs->insertTab(tabIndex, console, tabText);
    tabs->setCurrentIndex(tabIndex);
    tabs->setUpdatesEnabled(true);

    console->startShellProgram();

    tabs->setFocus();
    console->setFocus();

    if (oldWidget) {
        this->removeTermWidgetMapping(oldWidget);
        delete oldWidget;
    }
}

const QString MainWindow::getCurrentUsernameAndHost()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();

    if (connEntry == nullptr) {
        return QString("");
    }

    return connEntry->name;
}

std::shared_ptr<SSHConnectionEntry> MainWindow::getCurrentConnectionEntry()
{
    QModelIndexList indexes = this->connectionList->getSelection();
    if (indexes.isEmpty()) {
        return nullptr;
    }

    std::shared_ptr<SSHConnectionEntry> connEntry = this->connectionModel->getConnEntry(indexes.first().row());

    return connEntry;
}

std::shared_ptr<SSHConnectionEntry> MainWindow::getConnectionEntryByTermWidget(QTermWidget *console)
{
    return this->termToConn.at(console);
}

void MainWindow::removeTermWidgetMapping(QWidget *widget)
{
    if (widget->metaObject()->className() == QString("QTermWidget")) {
        QTermWidget *console = static_cast<QTermWidget *>(widget);
        this->termToConn.erase(console);
    }
}

CustomTabWidget* MainWindow::getCurrentTabWidget()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();
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
        this->removeTermWidgetMapping(termWidget);
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
    this->awsWidget->setRegion(settings.value("selectedAwsRegion", AWSConnector::LOCATION_US_EAST_1).toString());
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
        std::shared_ptr<SSHConnectionEntry> entry = std::make_shared<SSHConnectionEntry>();
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
        std::shared_ptr<SSHConnectionEntry> entry = this->connectionModel->getConnEntry(i);

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

void MainWindow::createSSHConnectionToAWS(std::shared_ptr<AWSInstance> instance,
        std::vector<std::shared_ptr<AWSInstance>> vpcNeighbours, bool toPrivateIP)
{
    this->newDialog->clear();

    if (toPrivateIP) {
        this->newDialog->setHostname(instance->privateIP);
        this->newDialog->setHopChecked(true);
    } else {
        this->newDialog->setHostname(instance->publicIP);
    }

    this->newDialog->addHopHost("", "");

    for (auto neighbour: vpcNeighbours) {
        QString label = "";
        if (!instance->name.isEmpty()) {
            label = neighbour->name + " (" + neighbour->publicIP + ")";
        } else {
            label = neighbour->id + " (" + neighbour->publicIP + ")";
        }

        this->newDialog->addHopHost(label, neighbour->publicIP);
    }

    this->newDialog->updateSSHKeys();

    this->newDialog->setSSHKey(this->findSSHKey(instance->keyname));
    this->newDialog->setFocusOnUsername();

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

void MainWindow::removeConnection()
{
    std::shared_ptr<SSHConnectionEntry> entry = this->getCurrentConnectionEntry();

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
        this->notesEditor->setEnabled(false);
        this->awsInfo->setAWSEnabled(false);
        this->sshSessionsStack->setCurrentIndex(0);
        this->setWindowTitle(PROGRAM_NAME);
    }

    entry.reset();
}

void MainWindow::updateConnectionTabs()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    this->setWindowTitle(connEntry->name + " - " + PROGRAM_NAME);

    this->machineInfo->setMachineEnabled(true);
    this->machineInfo->updateData(connEntry);

    this->notesEditor->setEnabled(true);
    this->notesEditor->setHtml(connEntry->notes);

    if (connEntry->isAwsInstance) {
        this->awsInfo->setAWSEnabled(true);
        this->awsInfo->updateData(connEntry->awsInstance);
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
        this->menuBar()->show();
        this->viewEnlarged = false;
    } else {
        this->sshSessionsStack->setParent(this->hiddenPage);
        this->hiddenPage->layout()->addWidget(this->sshSessionsStack);
        this->widgetStack->setCurrentIndex(1);
        this->menuBar()->hide();
        this->viewEnlarged = true;
    }

    this->toggleEnlarged->setChecked(this->viewEnlarged);
    this->setFocusOnCurrentTerminal();
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

    std::shared_ptr<SSHConnectionEntry> entry = this->connectionModel->getConnEntry(0);
    this->connectionList->selectConnection(entry);
}

void MainWindow::showPreferencesDialog()
{
    this->preferencesDialog->setFont(this->preferences.getTerminalFont());
    this->preferencesDialog->setColorScheme(this->preferences.getColorScheme());
    this->preferencesDialog->setAWSAccessKey(this->preferences.getAWSAccessKey());
    this->preferencesDialog->setAWSSecretKey(this->preferences.getAWSSecretKey());

    if (this->preferencesDialog->exec() == QDialog::Accepted) {
        // did the user change the font or the color scheme?
        if (this->preferencesDialog->getFont() != this->preferences.getTerminalFont() ||
                this->preferencesDialog->getColorScheme() != this->preferences.getColorScheme()) {
            this->preferences.setTerminalFont(this->preferencesDialog->getFont());
            this->preferences.setColorScheme(this->preferencesDialog->getColorScheme());
            this->updateConsoleSettings(this->preferences.getTerminalFont(),
                    this->preferences.getColorScheme());
        }

        // update the AWS credentials
        this->preferences.setAWSAccessKey(this->preferencesDialog->getAWSAccessKey());
        this->preferences.setAWSSecretKey(this->preferencesDialog->getAWSSecretKey());

        // save the preferences with QSettings
        this->preferences.save();
    }
}

void MainWindow::updateConsoleSettings(const QFont &font, const QString colorScheme)
{
    // update all running QTermWidget instances
    for (int i = 0; i < this->connectionModel->rowCount(QModelIndex()); i++) {
        std::shared_ptr<SSHConnectionEntry> entry = this->connectionModel->getConnEntry(i);
        for (int j = 0; j < entry->tabs->count(); j++) {
            QWidget *widget = entry->tabs->widget(j);
            if (widget->metaObject()->className() == QString("QTermWidget")) {
                QTermWidget *console = (QTermWidget*) widget;
                console->setTerminalFont(font);
                console->setColorScheme(colorScheme);
                console->update();
            }
        }
    }
}

void MainWindow::notesChanged()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    connEntry->notes = this->notesEditor->toHtml();
}

void MainWindow::nextTab()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();

    if (connEntry->tabs->currentIndex() < 0) {
        return;
    }

    if (connEntry->tabs->currentIndex() < (connEntry->tabs->count() - 1)) {
        connEntry->tabs->setCurrentIndex(connEntry->tabs->currentIndex() + 1);
    } else {
        connEntry->tabs->setCurrentIndex(0);
    }

    this->setFocusOnCurrentTerminal();
}

void MainWindow::prevTab()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getCurrentConnectionEntry();

    if (connEntry->tabs->currentIndex() < 0) {
        return;
    }

    if (connEntry->tabs->currentIndex() > 0) {
        connEntry->tabs->setCurrentIndex(connEntry->tabs->currentIndex() - 1);
    } else {
        connEntry->tabs->setCurrentIndex(connEntry->tabs->count() - 1);
    }

    this->setFocusOnCurrentTerminal();
}

void MainWindow::editConnection()
{
    auto connEntry = this->getCurrentConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    this->editDialog->setWindowTitle("Edit " + connEntry->name);
    this->editDialog->setHostname(connEntry->hostname);
    this->editDialog->setUsername(connEntry->username);
    this->editDialog->setShortDescription(connEntry->shortDescription);
    this->editDialog->setPassword(connEntry->password);
    this->editDialog->setSSHKey(connEntry->sshkey);
    this->editDialog->setPortNumber(connEntry->port);

    if (this->editDialog->exec() == QDialog::Rejected) {
        return;
    }

    connEntry->shortDescription = this->editDialog->getShortDescription();
    connEntry->sshkey = this->editDialog->getSSHKey();
    connEntry->password = this->editDialog->getPassword();
    connEntry->port = this->editDialog->getPortNumber();
}

void MainWindow::setFocusOnCurrentTerminal()
{
    auto connEntry = getCurrentConnectionEntry();
    if (connEntry == nullptr) {
        return;
    }

    if (connEntry->tabs == nullptr) {
        return;
    }

    QWidget *widget = connEntry->tabs->currentWidget();
    if (QString("QTermWidget") == widget->metaObject()->className()) {
        widget->setFocus();
    }
}

void MainWindow::dataReceived(const QString &text)
{
    QObject *sender = QObject::sender();
    if (sender->metaObject()->className() != QString("QTermWidget")) {
        return;
    }

    QTermWidget *console = static_cast<QTermWidget *>(sender);
    auto connEntry = this->getConnectionEntryByTermWidget(console);
    if (connEntry == nullptr) {
        return;
    }

    if (connEntry->password.isEmpty()) {
        return;
    }

    if (text.endsWith(" password: ")) {
        console->sendText(connEntry->password + "\n");
        QObject::disconnect(console, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }
}
