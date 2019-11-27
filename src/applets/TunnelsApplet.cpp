#include "TunnelsApplet.h"

TunnelsApplet::TunnelsApplet()
{
    this->firstShow = true;

    this->newDialog = new TunnelsNewDialog(this);
    connect(this->newDialog, &TunnelsNewDialog::accepted, this, &TunnelsApplet::createTunnel);

    this->toolBar = new QToolBar();

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, &TunnelsApplet::reloadData);
    this->toolBar->addAction(QIcon(":/images/applications-internet.svg"),
            "New Tunnel", this, &TunnelsApplet::showCreateTunnelDialog);
    this->toolBar->addSeparator();
    this->restartAction = this->toolBar->addAction(QIcon(":/images/edit-redo.svg"),
            "Restart Tunnel", this, &TunnelsApplet::restartTunnel);
    this->restartAction->setEnabled(false);
    this->shutdownAction = this->toolBar->addAction(QIcon(":/images/red-light.svg"),
            "Shutdown Tunnel", this, &TunnelsApplet::shutdownTunnel);
    this->shutdownAction->setEnabled(false);
    this->deleteAction = this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Delete Tunnel", this, &TunnelsApplet::removeTunnel);
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

    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table, &QTableView::customContextMenuRequested, this, &TunnelsApplet::showContextMenu);
    this->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    connect(this->table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TunnelsApplet::selectionChanged);

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

    if (tunnel == nullptr) {
        return;
    }

    if (tunnel->isConnected()) {
        QString connection = tunnel->username + "@" + tunnel->hostname;
        QString msg = QString("Do you really want to restart the SSH tunnel mapping port ") + QString::number(tunnel->remotePort) + " on '" +
            connection + "' to port " + QString::number(tunnel->localPort) + " on your machine?";

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Do you really want to restart this SSH tunnel?",
                msg, QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    int localPort = tunnel->localPort;
    int remotePort = tunnel->remotePort;

    TunnelManager::getInstance().restartTunnel(this->connEntry, this->connEntry->username, this->connEntry->hostname, localPort, remotePort);
    this->model->reloadData();
}

void TunnelsApplet::shutdownTunnel()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto tunnel = TunnelManager::getInstance().getTunnel(this->connEntry->username, this->connEntry->hostname, row);

    if (tunnel == nullptr) {
        return;
    }

    if (tunnel->isConnected()) {
        QString connection = tunnel->username + "@" + tunnel->hostname;
        QString msg = QString("Do you really want to shutdown the SSH tunnel mapping port ") + QString::number(tunnel->remotePort) + " on '" +
            connection + "' to port " + QString::number(tunnel->localPort) + " on your machine?";

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Do you really want to shutdown this SSH tunnel?",
                msg, QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("Can't shutdown tunnel because it is already down.");
        msgBox.exec();
    }

    int localPort = tunnel->localPort;
    int remotePort = tunnel->remotePort;

    TunnelManager::getInstance().shutdownTunnel(this->connEntry, this->connEntry->username, this->connEntry->hostname, localPort, remotePort);
    this->model->reloadData();
}

void TunnelsApplet::removeTunnel()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto tunnel = TunnelManager::getInstance().getTunnel(this->connEntry->username, this->connEntry->hostname, row);

    if (tunnel == nullptr) {
        return;
    }

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
    this->selectionChanged();
}

int TunnelsApplet::getSelectedRow()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return -1;
    }

    return indexes.first().row();
}

void TunnelsApplet::selectionChanged()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();

    if (indexes.size() == 0) {
        this->restartAction->setEnabled(false);
        this->shutdownAction->setEnabled(false);
        this->deleteAction->setEnabled(false);

        return;
    }

    this->deleteAction->setEnabled(true);
    this->shutdownAction->setEnabled(true);
    this->restartAction->setEnabled(true);
}

void TunnelsApplet::showContextMenu(QPoint pos)
{
    if (!this->table->indexAt(pos).isValid()) {
        return;
    }

    QMenu menu;
    menu.addAction(this->restartAction);
    menu.addAction(this->shutdownAction);
    menu.addSeparator();
    menu.addAction(this->deleteAction);

    QPoint globalPos = this->table->mapToGlobal(pos);
    menu.exec(globalPos);
}
