#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowIcon(QIcon(":/images/utilities-terminal.svg"));

    this->viewEnlarged = false;
    this->awsConsoleDetached = false;

    // read in the user preferences from QSettings
    Preferences &preferences = Preferences::getInstance();
    preferences.read();

    // dialogs
    this->aboutDialog = new AboutDialog();
    this->newDialog = new NewDialog();
    this->preferencesDialog = new PreferencesDialog();
    connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

    // build the menu bar
    QMenuBar *menuBar = new QMenuBar(0);

    QMenu *connMenu = new QMenu(tr("Connection"));
    QAction *newRole = connMenu->addAction(QIcon(":/images/applications-internet.svg"), tr("&New"), this, SLOT(showNewDialog()));
    newRole->setMenuRole(QAction::NoRole);
    connMenu->addSeparator();
    QAction *quitRole = connMenu->addAction(QIcon(":/images/system-log-out.svg"), tr("&Quit"), qApp, SLOT(quit()));
    quitRole->setMenuRole(QAction::QuitRole);

    QMenu *editMenu = new QMenu(tr("Edit"));
    QAction *preferencesAction = editMenu->addAction(QIcon(":/images/preferences-system.svg"),
                tr("Preferences"), this, SLOT(showPreferencesDialog()));
    preferencesAction->setMenuRole(QAction::PreferencesRole);

    QMenu *helpMenu = new QMenu(tr("Help"));
    QAction *websiteAction = helpMenu->addAction(QIcon(":/images/go-home.svg"),
            tr("Website"), this, SLOT(openWebsite()));
    websiteAction->setMenuRole(QAction::NoRole);
    helpMenu->addSeparator();
    QAction *aboutAction = helpMenu->addAction(QIcon(":/images/help-browser.svg"),
            tr("About"), this->aboutDialog, SLOT(exec()));
    aboutAction->setMenuRole(QAction::ApplicationSpecificRole);

    menuBar->addMenu(connMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(helpMenu);

    this->setMenuBar(menuBar);

    this->widgetStack = new QStackedWidget();

    this->splitter = new QSplitter(Qt::Horizontal);
    this->splitter->setContentsMargins(0, 0, 0, 0);

    // create the connection list and its model
    this->connectionModel = new SSHConnectionItemModel();
    this->connectionList = new ConnectionListWidget(this->connectionModel);

    connect(this->connectionList, SIGNAL(newDialogRequested()), this, SLOT(showNewDialog()));
    connect(this->connectionList, SIGNAL(connectionRemoved(std::shared_ptr<SSHConnectionEntry>)),
            this, SLOT(connectionRemoved(std::shared_ptr<SSHConnectionEntry>)));
    connect(this->connectionList, SIGNAL(connectionChanged(int)),
            this, SLOT(changeConnection(int)));
    connect(this->connectionList, SIGNAL(toggleAwsConsole(bool)), this, SLOT(toggleAwsConsole(bool)));

    this->splitter->addWidget(this->connectionList);

    this->terminalViewContainer = new QWidget();
    this->terminalViewContainer->setLayout(new QVBoxLayout());

    this->terminalView = new TerminalViewWidget();
    this->terminalViewContainer->layout()->addWidget(this->terminalView);
    this->terminalViewContainer->layout()->setContentsMargins(0, 0, 0, 0);
    connect(this->terminalView, SIGNAL(requestToggleEnlarge()), this, SLOT(toggleEnlargeWidget()));

    // create the stacked widget that allows to switch between SSH and AWS
    this->rightWidget = new QStackedWidget();
    rightWidget->addWidget(this->terminalViewContainer);

    this->awsWidget = new AWSWidget();
    connect(this->awsWidget, SIGNAL(newConnection(std::shared_ptr<AWSInstance>,
            std::vector<std::shared_ptr<AWSInstance>>, bool)), this,
            SLOT(createSSHConnectionToAWS(std::shared_ptr<AWSInstance>,
            std::vector<std::shared_ptr<AWSInstance>>, bool)));
    connect(this->awsWidget, SIGNAL(awsInstancesUpdated()), this->connectionList, SLOT(updateAWSInstances()));
    rightWidget->addWidget(this->awsWidget);

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

    this->setCentralWidget(this->widgetStack);

    this->awsConsoleWindow = new AWSConsoleWindow();
    connect(this->awsWidget, SIGNAL(requestToggleDetach(bool)), this, SLOT(toggleDetachAwsConsole(bool)));
    connect(this->awsConsoleWindow, SIGNAL(requestReattach()), this->awsWidget, SLOT(reattach()));

    this->readSettings();

    this->awsWidget->loadData();
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

void MainWindow::changeConnection(int row)
{
    this->terminalView->setCurrentConnection(row);
    this->updateConnectionTabs();

    this->terminalView->setFocusOnCurrentTerminal();
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
    connEntry->hopUsernames = this->newDialog->getHopUsernames();
    connEntry->hopSSHKeys = this->newDialog->getHopSSHKeys();

    if (this->newDialog->isAwsInstance) {
        connEntry->isAwsInstance = true;
        connEntry->awsInstance = this->newDialog->awsInstance;
    }

    this->connectionModel->appendConnectionEntry(connEntry);
    this->connectionList->selectLast();

    TabbedTerminalWidget *tabs = new TabbedTerminalWidget(connEntry);
    connEntry->tabs = tabs;
    tabs->addTerminalSession();
    this->terminalView->addConnection(connEntry, tabs);
    this->terminalView->setLastConnection();

    this->terminalView->setDisabledPageEnabled(false);
    this->rightWidget->setCurrentIndex(0);
    this->connectionList->resetAWSConsoleButton();

    this->terminalView->setFocusOnCurrentTerminal();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::shared_ptr<SSHConnectionEntry> connEntry;
    for (int i = 0; (connEntry = this->connectionModel->getConnEntry(i)); i++) {
        connEntry->tabs->closeAllDetachedWindows();
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

        TabbedTerminalWidget *tabs = new TabbedTerminalWidget(entry);

        for (int i = 0; i < entry->tabNames->size(); i++) {
            tabs->addInactiveSession(entry->tabNames->at(i));
        }

        this->terminalView->addConnection(entry, tabs);

        entry->tabs = tabs;

        this->connectionModel->appendConnectionEntry(entry);

        this->terminalView->setDisabledPageEnabled(false);
        this->rightWidget->setCurrentIndex(0);
        this->connectionList->selectFirstConnection();
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

    this->newDialog->setSSHKey(NewDialog::findSSHKey(instance->keyname));
    this->newDialog->setShortDescription(instance->name);
    this->newDialog->setFocusOnUsername();

    this->newDialog->isAwsInstance = true;
    this->newDialog->awsInstance = instance;
    this->newDialog->awsInstance->resolveReferences();
    this->newDialog->exec();
    this->newDialog->isAwsInstance = false;
}

void MainWindow::connectionRemoved(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->terminalView->removeConnection(connEntry->tabs);

    if (this->connectionModel->rowCount(QModelIndex()) == 0) {
        this->terminalView->setDisabledPageEnabled(true);
        this->setWindowTitle(PROGRAM_NAME);
    }
}

void MainWindow::updateConnectionTabs()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->connectionList->getSelectedConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    this->setWindowTitle(connEntry->name + " - " + PROGRAM_NAME);
}

void MainWindow::openWebsite()
{
    QDesktopServices::openUrl(QUrl("https://github.com/aheck/sshcommander"));
}

void MainWindow::showPreferencesDialog()
{
    Preferences &preferences = Preferences::getInstance();
    this->preferencesDialog->setFont(preferences.getTerminalFont());
    this->preferencesDialog->setColorScheme(preferences.getColorScheme());
    this->preferencesDialog->setAWSAccessKey(preferences.getAWSAccessKey());
    this->preferencesDialog->setAWSSecretKey(preferences.getAWSSecretKey());

    if (this->preferencesDialog->exec() == QDialog::Accepted) {
        // did the user change the font or the color scheme?
        if (this->preferencesDialog->getFont() != preferences.getTerminalFont() ||
                this->preferencesDialog->getColorScheme() != preferences.getColorScheme()) {
            preferences.setTerminalFont(this->preferencesDialog->getFont());
            preferences.setColorScheme(this->preferencesDialog->getColorScheme());
            this->terminalView->updateConsoleSettings(preferences.getTerminalFont(),
                    preferences.getColorScheme());
        }

        // update the AWS credentials
        preferences.setAWSAccessKey(this->preferencesDialog->getAWSAccessKey());
        preferences.setAWSSecretKey(this->preferencesDialog->getAWSSecretKey());

        // save the preferences with QSettings
       preferences.save();
    }
}

void MainWindow::toggleEnlargeWidget()
{
    if (this->connectionList->getSelectedConnectionEntry() == nullptr) {
        return;
    }

    QObject *obj = QObject::sender();
    QWidget *widget = qobject_cast<QWidget *>(obj);

    if (widget == nullptr) {
        return;
    }

    if (this->viewEnlarged) {
        this->hiddenPage->layout()->removeWidget(widget);
        this->terminalViewContainer->layout()->addWidget(widget);
        this->widgetStack->setCurrentIndex(0);
        this->menuBar()->show();

        this->viewEnlarged = false;
    } else {
        widget->setParent(this->hiddenPage);
        this->hiddenPage->layout()->addWidget(widget);
        this->widgetStack->setCurrentIndex(1);
        this->menuBar()->hide();

        this->viewEnlarged = true;
    }
}

void MainWindow::toggleAwsConsole(bool show)
{
    if (show) {
        rightWidget->setCurrentIndex(1);
    } else {
        rightWidget->setCurrentIndex(0);
    }
}

void MainWindow::toggleDetachAwsConsole(bool detach)
{
    if (detach) {
        this->awsWidget->setParent(this->awsConsoleWindow);
        this->awsConsoleWindow->layout()->addWidget(this->awsWidget);
        this->rightWidget->setCurrentIndex(0);
        this->awsWidget->show();
        this->awsConsoleWindow->show();

        this->connectionList->disableAWSConsoleButton();
    } else {
        this->awsConsoleWindow->layout()->removeWidget(this->awsWidget);
        this->awsWidget->setParent(this->rightWidget);
        this->rightWidget->addWidget(this->awsWidget);
        this->rightWidget->setCurrentIndex(1);
        this->awsConsoleWindow->hide();

        this->connectionList->enableAWSConsoleButton();
    }
}
