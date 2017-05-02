#include "ProcessesApplet.h"

ProcessesApplet::ProcessesApplet()
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
    this->model = new ProcessesItemModel();
    this->table->setModel(this->model);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    this->layout()->addWidget(this->table);
}

const QString ProcessesApplet::getDisplayName()
{
    return tr("Processes");
}

QIcon ProcessesApplet::getIcon()
{
    return QIcon(":/images/network-wired.svg");
}

void ProcessesApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
}

void ProcessesApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void ProcessesApplet::updateData()
{
    this->model->clear();

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "ps ax", this, "sshResultReceived");
}

void ProcessesApplet::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
{
    if (!cmdResult->isSuccess) {
        std::cout << "ERROR: SSH remote command failed: " << cmdResult->errorString.toStdString() << std::endl;
        return;
    }

    this->model->updateData(cmdResult->resultString);
}

void ProcessesApplet::reloadData()
{
    this->updateData();
}
