#include "FileTransfersApplet.h"

FileTransfersApplet::FileTransfersApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->addSeparator();
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Cancel File Transfer", this, SLOT(cancelFileTransfer()));
    this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Remove File Transfer", this, SLOT(removeFileTransfer()));

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->model = new FileTransfersItemModel();
    this->table->setModel(this->model);

    this->layout()->addWidget(this->table);
}

FileTransfersApplet::~FileTransfersApplet()
{
}

const QString FileTransfersApplet::getDisplayName()
{
    return tr("File Transfers");
}

QIcon FileTransfersApplet::getIcon()
{
    return QIcon(":/images/document-save.svg");
}

void FileTransfersApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    this->model->setConnectionId(this->connEntry->getIdentifier());
}

void FileTransfersApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
}

int FileTransfersApplet::getSelectedRow()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return -1;
    }

    return indexes.first().row();
}

void FileTransfersApplet::reloadData()
{
    this->model->reloadData();
}

void FileTransfersApplet::jobDataChanged(QUuid uuid)
{
    this->model->jobDataChanged(uuid);
}

void FileTransfersApplet::cancelFileTransfer()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    auto job = SSHConnectionManager::getInstance().getFileTransferJob(this->connEntry->getIdentifier(), row);
    if (job == nullptr) {
        job->cancelationRequested = true;
    }
}

void FileTransfersApplet::removeFileTransfer()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    SSHConnectionManager::getInstance().removeFileTransferJob(this->connEntry->getIdentifier(), row);
}
