#include "TunnelsApplet.h"

TunnelsApplet::TunnelsApplet()
{
    this->firstShow = true;

    this->newDialog = new TunnelsNewDialog(this);
    connect(this->newDialog, SIGNAL(accepted()), this, SLOT(createTunnel()));

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->addAction(QIcon(":/images/applications-internet.svg"),
            "New Tunnel", this, SLOT(showCreateTunnelDialog()));
    this->toolBar->addSeparator();
    this->restartAction = this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Restart Tunnel", this, SLOT(restartTunnel()));
    this->restartAction->setEnabled(false);
    this->deleteAction = this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Delete Tunnel", this, SLOT(removeTunnel()));
    this->deleteAction->setEnabled(false);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->table->setSortingEnabled(false);
    this->model = new TunnelsItemModel();
    this->table->setModel(this->model);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);

    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }

    connect(this->table->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection, QItemSelection)));

    this->layout()->addWidget(this->table);
}

const QString TunnelsApplet::getDisplayName()
{
    return tr("Tunnels");
}

QIcon TunnelsApplet::getIcon()
{
    return QIcon(":/images/network-wired.svg");
}

void TunnelsApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    this->model->setConnectionStrings(this->connEntry->username, this->connEntry->hostname);
}

void TunnelsApplet::onShow()
{
    this->reloadData();

    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void TunnelsApplet::updateData()
{
    this->model->clear();
}

void TunnelsApplet::showCreateTunnelDialog()
{
    this->newDialog->clear();
    this->newDialog->exec();
}

void TunnelsApplet::createTunnel()
{
    int localPort = this->newDialog->getLocalPort();
    int remotePort = this->newDialog->getRemotePort();
    QString shortDescription = this->newDialog->getShortDescription();

    TunnelManager::getInstance().createTunnel(this->connEntry, localPort, remotePort, shortDescription);
    this->model->reloadData();
}

void TunnelsApplet::reloadData()
{
    this->updateData();
}

void TunnelsApplet::restartTunnel()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto tunnel = TunnelManager::getInstance().getTunnel(this->connEntry->username, this->connEntry->hostname, row);

    QString connection = tunnel->username + "@" + tunnel->hostname;
    QString msg = QString("Do you really want to restart the SSH tunnel mapping port ") + QString::number(tunnel->remotePort) + " on '" +
        connection + "' to port " + QString::number(tunnel->localPort) + " on your machine?";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Do you really want to restart this SSH tunnel?",
            msg, QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    int localPort = tunnel->localPort;
    int remotePort = tunnel->remotePort;

    TunnelManager::getInstance().restartTunnel(this->connEntry, this->connEntry->username, this->connEntry->hostname, localPort, remotePort);
    this->model->reloadData();
}

void TunnelsApplet::removeTunnel()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto tunnel = TunnelManager::getInstance().getTunnel(this->connEntry->username, this->connEntry->hostname, row);

    QString connection = tunnel->username + "@" + tunnel->hostname;
    QString msg = QString("Do you really want to delete the SSH tunnel mapping port ") + QString::number(tunnel->remotePort) + " on '" +
        connection + "' to port " + QString::number(tunnel->localPort) + " on your machine?";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Do you really want to delete this SSH tunnel?",
            msg, QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    int localPort = tunnel->localPort;
    int remotePort = tunnel->remotePort;

    TunnelManager::getInstance().removeTunnel(this->connEntry->username, this->connEntry->hostname, localPort, remotePort);
    this->model->reloadData();
}

int TunnelsApplet::getSelectedRow()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return -1;
    }

    return indexes.first().row();
}

void TunnelsApplet::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();

    if (indexes.size() == 0) {
        this->deleteAction->setEnabled(false);
        this->restartAction->setEnabled(false);

        return;
    }

    this->deleteAction->setEnabled(true);
    this->restartAction->setEnabled(true);
}
