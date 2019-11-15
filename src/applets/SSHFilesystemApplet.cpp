#include "SSHFilesystemApplet.h"

SSHFilesystemApplet::SSHFilesystemApplet()
{
    this->sshfsNotInstalled = false;
    this->firstShow = true;

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);

    // check if sshfs is installed
    QString sshfsPath = ExternalProgramFinder::getSSHFSPath();
    if (sshfsPath.isEmpty()) {
        this->sshfsNotInstalled = true;
        DisabledWidget *disabledWidget = new DisabledWidget("sshfs is not installed on your machine");
        disabledWidget->setDescription("Install sshfs and restart SSH Commander to use this feature");
        this->layout()->addWidget(disabledWidget);
        return;
    }

    this->newDialog = new SSHFilesystemNewDialog(this);
    connect(this->newDialog, SIGNAL(accepted()), this, SLOT(createNewMountEntry()));

    this->toolBar = new QToolBar();

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->addAction(QIcon(":/images/applications-internet.svg"),
            "Create New Mount", this, SLOT(showNewDialog()));
    this->toolBar->addSeparator();
    this->openAction = this->toolBar->addAction(QIcon(":/images/folder-open.svg"),
            "Open Directory", this, SLOT(openDirectory()));
    this->openAction->setEnabled(false);
    this->mountAction = this->toolBar->addAction(QIcon(":/images/go-next.svg"),
            "Mount", this, SLOT(mountMountEntry()));
    this->mountAction->setEnabled(false);
    this->unmountAction = this->toolBar->addAction(QIcon(":/images/go-top.svg"),
            "Unmount", this, SLOT(unmountMountEntry()));
    this->unmountAction->setEnabled(false);
    this->deleteAction = this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Delete Mount", this, SLOT(removeMountEntry()));
    this->deleteAction->setEnabled(false);

    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->table->setSortingEnabled(false);
    this->model = new SSHFilesystemItemModel();
    this->table->setModel(this->model);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this->table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openDirectory()));

    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    this->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    connect(this->table->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged()));

    this->layout()->addWidget(this->table);
}

const QString SSHFilesystemApplet::getDisplayName()
{
    return tr("SSHFS");
}

QIcon SSHFilesystemApplet::getIcon()
{
    return QIcon(":/images/folder-remote.svg");
}

void SSHFilesystemApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    if (this->sshfsNotInstalled) {
        return;
    }

    this->newDialog->setConnEntry(connEntry);

    this->model->setConnectionStrings(this->connEntry->username, this->connEntry->hostname);
}

void SSHFilesystemApplet::onShow()
{
    if (this->sshfsNotInstalled) {
        return;
    }

    this->reloadData();

    if (!this->firstShow) {
        return;
    }

    this->table->resizeColumnsToContents();
    this->firstShow = false;
    this->updateData();
}

void SSHFilesystemApplet::updateData()
{
    this->model->reloadData();
}

void SSHFilesystemApplet::showNewDialog()
{
    this->newDialog->clear();
    this->newDialog->exec();
}

void SSHFilesystemApplet::reloadData()
{
    this->updateData();
}

int SSHFilesystemApplet::getSelectedRow()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return -1;
    }

    return indexes.first().row();
}

void SSHFilesystemApplet::selectionChanged()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();

    if (indexes.size() == 0) {
        this->openAction->setEnabled(false);
        this->mountAction->setEnabled(false);
        this->deleteAction->setEnabled(false);
        this->unmountAction->setEnabled(false);

        return;
    }

    this->openAction->setEnabled(true);
    this->mountAction->setEnabled(true);
    this->deleteAction->setEnabled(true);
    this->unmountAction->setEnabled(true);
}

void SSHFilesystemApplet::createNewMountEntry()
{
    SSHFilesystemManager::getInstance().createMountEntry(this->connEntry,
            this->newDialog->getLocalDir(), this->newDialog->getRemoteDir(),
            this->newDialog->getShortDescription());

    this->model->reloadData();
}

void SSHFilesystemApplet::openDirectory()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto mountEntry = SSHFilesystemManager::getInstance().getMountEntry(this->connEntry->username, this->connEntry->hostname, row);
    if (mountEntry == nullptr) {
        return;
    }

    QDesktopServices::openUrl(QUrl("file://" + mountEntry->localDir));
}

void SSHFilesystemApplet::mountMountEntry()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto mountEntry = SSHFilesystemManager::getInstance().getMountEntry(this->connEntry->username, this->connEntry->hostname, row);

    mountEntry->mount(this->connEntry);
    this->model->reloadData();
}

void SSHFilesystemApplet::unmountMountEntry()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto mountEntry = SSHFilesystemManager::getInstance().getMountEntry(this->connEntry->username, this->connEntry->hostname, row);

    mountEntry->unmount();
    this->model->reloadData();
}

void SSHFilesystemApplet::removeMountEntry()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto mountEntry = SSHFilesystemManager::getInstance().getMountEntry(this->connEntry->username, this->connEntry->hostname, row);

    QString connectionId = mountEntry->username + "@" + mountEntry->hostname;

    QString localDir = mountEntry->localDir;
    QString remoteDir = mountEntry->remoteDir;

    QString msg = "Do you really want to delete the SSHFS mount entry mounting " + connectionId + ":" + remoteDir
        + " to " + localDir +  " on your machine?";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Do you really want to delete this SSH mount entry?",
            msg, QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    SSHFilesystemManager::getInstance().removeMountEntry(this->connEntry->username, this->connEntry->hostname, localDir, remoteDir);
    this->model->reloadData();
    this->selectionChanged();
}

void SSHFilesystemApplet::showContextMenu(QPoint pos)
{
    if (!this->table->indexAt(pos).isValid()) {
        return;
    }

    QMenu menu;
    menu.addAction(this->openAction);
    menu.addAction(this->mountAction);
    menu.addAction(this->unmountAction);
    menu.addSeparator();
    menu.addAction(this->deleteAction);

    QPoint globalPos = this->table->mapToGlobal(pos);
    menu.exec(globalPos);
}
