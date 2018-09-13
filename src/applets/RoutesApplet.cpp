#include "RoutesApplet.h"

RoutesApplet::RoutesApplet()
{
    this->firstShow = true;
    this->toolBar = new QToolBar();

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->setOrientation(Qt::Vertical);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->model = new RoutesItemModel();
    this->table->setModel(this->model);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->setSortingEnabled(true);
    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    this->layout()->addWidget(this->table);
}

const QString RoutesApplet::getDisplayName()
{
    return tr("Routes");
}

QIcon RoutesApplet::getIcon()
{
    return QIcon(":/images/network-wired.svg");
}

void RoutesApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
}

void RoutesApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void RoutesApplet::updateData()
{
    this->model->clear();

    if (this->connEntry->osType != OSType::Linux) {
        return;
    }

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "route -n", this, "sshResultReceived");
}

void RoutesApplet::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
{
    if (!cmdResult->isSuccess) {
        if (cmdResult->commandNotFound) {
            QMessageBox msgBox;
            msgBox.setText("Failed to execute route command. Is package net-tools installed on " +
                    this->connEntry->hostname + "?");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            return;
        }

        std::cout << "ERROR: SSH remote command failed: " << cmdResult->errorString.toStdString() << std::endl;
        return;
    }

    this->model->updateData(cmdResult->resultString);
}

void RoutesApplet::reloadData()
{
    this->updateData();
}
