#include "PortsApplet.h"

PortsApplet::PortsApplet()
{
    this->firstShow = true;
    this->toolBar = new QToolBar();
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->setOrientation(Qt::Vertical);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->setItemDelegateForColumn(static_cast<int>(PortColumns::Details), new RichTextDelegate());
    this->table->setSortingEnabled(true);
    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    this->model = new PortsItemModel();
    this->table->setModel(this->model);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }

    this->newDialog = new PortsNewTunnelDialog(this);
    connect(this->newDialog, SIGNAL(accepted()), this, SLOT(createTunnel()));

    this->layout()->addWidget(this->table);
}

const QString PortsApplet::getDisplayName()
{
    return tr("Ports");
}

QIcon PortsApplet::getIcon()
{
    return QIcon(":/images/network-wired.svg");
}

void PortsApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
}

void PortsApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void PortsApplet::updateData()
{
    this->model->clear();

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "netstat -lntu", this, "sshResultReceived");
}

void PortsApplet::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
{
    if (!cmdResult->isSuccess) {
        std::cout << "ERROR: SSH remote command failed: " << cmdResult->errorString.toStdString() << std::endl;
        return;
    }

    this->model->updateData(cmdResult->resultString);

    for (int i = 0; i < this->model->rowCount(QModelIndex()); i++) {
        QModelIndex index = this->model->index(i, 3, QModelIndex());
        this->table->openPersistentEditor(index);
    }
}

void PortsApplet::reloadData()
{
    this->updateData();
}

void PortsApplet::showContextMenu(QPoint pos)
{
    QPoint globalPos = this->table->mapToGlobal(pos);
    QModelIndex index = this->table->indexAt(pos);

    if (index.isValid()) {
        QMenu menu;

        //std::shared_ptr<AWSInstance> instance = this->getSelectedInstance();
        menu.addAction(tr("Tunnel this port to local machine"), this, SLOT(showNewTunnelDialog()));

        menu.addSeparator();

        QMenu *openWithProgramActions = menu.addMenu(tr("Open with..."));

        QAction *pgOpenAction = openWithProgramActions->addAction(tr("..."), this, SLOT(startInstance()));

        menu.exec(globalPos);
    }
}

void PortsApplet::showNewTunnelDialog()
{
    int row = this->getSelectedRow();
    std::shared_ptr<NetstatEntry> netstatEntry = this->model->getNetstatEntry(row);
    int remotePort = netstatEntry->localPort.toInt(nullptr, 10);

    this->newDialog->clear();
    this->newDialog->setRemoteHostname(this->connEntry->hostname);
    this->newDialog->setRemotePort(remotePort);
    this->newDialog->update();
    this->newDialog->show();
}

void PortsApplet::createTunnel()
{
    int localPort = this->newDialog->getLocalPort();
    int remotePort = this->newDialog->getRemotePort();
    QString shortDescription = this->newDialog->getShortDescription();

    TunnelManager::getInstance().createTunnel(this->connEntry, localPort, remotePort, shortDescription);
}

int PortsApplet::getSelectedRow()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return -1;
    }

    return indexes.first().row();
}
