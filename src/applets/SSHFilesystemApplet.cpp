#include "SSHFilesystemApplet.h"

SSHFilesystemApplet::SSHFilesystemApplet()
{
    this->firstShow = true;

    this->newDialog = new SSHFilesystemNewDialog(this);
    connect(this->newDialog, SIGNAL(accepted()), this, SLOT(createNewMountEntry()));

    this->toolBar = new QToolBar();
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
    this->restartAction = this->toolBar->addAction(QIcon(":/images/go-top.svg"),
            "Unmount", this, SLOT(unmountMountEntry()));
    this->restartAction->setEnabled(false);
    this->deleteAction = this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Delete Mount", this, SLOT(removeMountEntry()));
    this->deleteAction->setEnabled(false);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->table->setSortingEnabled(false);
    this->model = new SSHFilesystemItemModel();
    this->table->setModel(this->model);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(this->table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openDirectory()));

    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }

    connect(this->table->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection, QItemSelection)));

    this->layout()->addWidget(this->table);
}

const QString SSHFilesystemApplet::getDisplayName()
{
    return tr("SSHFS");
}

QIcon SSHFilesystemApplet::getIcon()
{
    return QIcon(":/images/drive-harddisk.svg");
}

void SSHFilesystemApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    this->model->setConnectionStrings(this->connEntry->username, this->connEntry->hostname);
}

void SSHFilesystemApplet::onShow()
{
    this->reloadData();

    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void SSHFilesystemApplet::updateData()
{
    this->model->clear();
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

void SSHFilesystemApplet::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();

    if (indexes.size() == 0) {
        this->openAction->setEnabled(false);
        this->mountAction->setEnabled(false);
        this->deleteAction->setEnabled(false);
        this->restartAction->setEnabled(false);

        return;
    }

    this->openAction->setEnabled(true);
    this->mountAction->setEnabled(true);
    this->deleteAction->setEnabled(true);
    this->restartAction->setEnabled(true);
}

void SSHFilesystemApplet::createNewMountEntry()
{
    SSHFilesystemManager::getInstance().mountFilesystem(this->connEntry,
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

    QDesktopServices::openUrl(QUrl("file://" + mountEntry->localDir));
}

void SSHFilesystemApplet::mountMountEntry()
{

}

void SSHFilesystemApplet::unmountMountEntry()
{

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
}
