#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->viewEnlarged = false;
    this->enlargedWidget = nullptr;

    // read in the user preferences from QSettings
    this->preferences.read();

    // dialogs
    this->aboutDialog = new AboutDialog();
    this->newDialog = new NewDialog();
    this->preferencesDialog = new PreferencesDialog();
    connect(newDialog, SIGNAL (accepted()), this, SLOT (createNewConnection()));

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

    // create the connection list and its model
    this->connectionModel = new SSHConnectionItemModel();
    this->connectionList = new ConnectionListWidget(this->connectionModel);

    connect(this->connectionList, SIGNAL(newDialogRequested()), this, SLOT(showNewDialog()));
    connect(this->connectionList, SIGNAL(connectionRemoved(std::shared_ptr<SSHConnectionEntry>)),
            this, SLOT(connectionRemoved(std::shared_ptr<SSHConnectionEntry>)));
    connect(this->connectionList, SIGNAL(connectionChanged(int)),
            this, SLOT(changeConnection(int)));

    this->splitter->addWidget(this->connectionList);

    this->sessionInfoSplitter = new QSplitter(Qt::Vertical);

    this->terminalView = new TerminalViewWidget();
    this->sessionInfoSplitter->addWidget(this->terminalView);
    connect(this->terminalView, SIGNAL(requestToggleEnlarge()), this, SLOT(toggleEnlargeWidget()));

    this->notesEditor = new NotesEditor();
    connect(this->notesEditor, SIGNAL(textChanged()), this, SLOT(notesChanged()));

    // create the tab where the applets reside
    this->appletTab = new QTabWidget();
    this->appletTab->addTab(this->machineInfo, "Machine");
    this->appletTab->addTab(this->notesEditor, "Notes");
    this->appletTab->addTab(this->awsInfo, "AWS");

    this->sessionInfoSplitter->addWidget(this->appletTab);
    this->sessionInfoSplitter->setStretchFactor(0, 10);
    this->sessionInfoSplitter->setStretchFactor(1, 5);
    this->sessionInfoSplitter->setCollapsible(0, false);

    this->rightWidget = new QTabWidget();
    rightWidget->addTab(this->sessionInfoSplitter, "SSH");

    this->awsWidget = new AWSWidget(&this->preferences);
    connect(this->awsWidget, SIGNAL(newConnection(std::shared_ptr<AWSInstance>,
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

    if (this->newDialog->isAwsInstance) {
        connEntry->isAwsInstance = true;
        connEntry->awsInstance = this->newDialog->awsInstance;
    }

    this->connectionModel->appendConnectionEntry(connEntry);
    this->connectionList->selectLast();

    TabbedTerminalWidget *tabs = new TabbedTerminalWidget(&this->preferences, connEntry);
    connEntry->tabs = tabs;
    tabs->addTerminalSession();
    this->terminalView->addConnection(tabs);
    this->terminalView->setLastConnection();

    this->terminalView->setDisabledPageEnabled(false);
    this->rightWidget->setCurrentIndex(0);

    this->terminalView->setFocusOnCurrentTerminal();
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

        TabbedTerminalWidget *tabs = new TabbedTerminalWidget(&this->preferences, entry);

        for (int i = 0; i < entry->tabNames->size(); i++) {
            tabs->addInactiveSession(entry->tabNames->at(i));
        }

        this->terminalView->addConnection(tabs);

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

    this->newDialog->setSSHKey(NewDialog::findSSHKey(instance->keyname));
    this->newDialog->setFocusOnUsername();

    this->newDialog->isAwsInstance = true;
    this->newDialog->awsInstance = instance;
    this->newDialog->exec();
    this->newDialog->isAwsInstance = false;
}

void MainWindow::connectionRemoved(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    TabbedTerminalWidget *tabWidget = connEntry->tabs;
    for (int i = tabWidget->count(); i >= 0; --i) {
        QTermWidget *termWidget = (QTermWidget*) tabWidget->widget(i);
        tabWidget->removeTab(i);
        delete termWidget;
    }

    this->terminalView->removeConnection(tabWidget);

    if (this->connectionModel->rowCount(QModelIndex()) == 0) {
        this->machineInfo->setMachineEnabled(false);
        this->notesEditor->setEnabled(false);
        this->awsInfo->setAWSEnabled(false);
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

void MainWindow::openWebsite()
{
    QDesktopServices::openUrl(QUrl("https://github.com/aheck/sshcommander"));
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

void MainWindow::notesChanged()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->connectionList->getSelectedConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    connEntry->notes = this->notesEditor->toHtml();
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
        this->sessionInfoSplitter->insertWidget(0, widget);
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
