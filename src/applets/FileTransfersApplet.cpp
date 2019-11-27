#include "FileTransfersApplet.h"

FileTransfersApplet::FileTransfersApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, &FileTransfersApplet::reloadData);
    this->toolBar->addSeparator();
    this->restartAction = this->toolBar->addAction(QIcon(":/images/edit-redo.svg"),
            "Restart File Transfer", this, &FileTransfersApplet::restartFileTransfer);
    this->restartAction->setEnabled(false);
    this->cancelAction = this->toolBar->addAction(QIcon(":/images/red-light.svg"),
            "Cancel File Transfer", this, &FileTransfersApplet::cancelFileTransfer);
    this->cancelAction->setEnabled(false);
    this->removeAction = this->toolBar->addAction(QIcon(":/images/process-stop.svg"),
            "Remove File Transfer", this, &FileTransfersApplet::removeFileTransfer);
    this->removeAction->setEnabled(false);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableView(this);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->model = new FileTransfersItemModel();
    this->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    this->table->setModel(this->model);

    connect(this->table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FileTransfersApplet::selectionChanged);

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

    emit changed();
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
    if (job != nullptr) {
        job->cancelationRequested = true;
    }
}

void FileTransfersApplet::restartFileTransfer()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    SSHConnectionManager::getInstance().restartFileTransferJob(this->connEntry->getIdentifier(), row);
}

void FileTransfersApplet::removeFileTransfer()
{
    int row = this->getSelectedRow();

    if (row < 0) {
        return;
    }

    this->model->removeFileTransferJob(row);
}

void FileTransfersApplet::selectionChanged()
{
    QModelIndexList indexes = this->table->selectionModel()->selectedIndexes();

    if (indexes.size() == 0) {
        this->cancelAction->setEnabled(false);
        this->restartAction->setEnabled(false);
        this->removeAction->setEnabled(false);

        return;
    }

    this->cancelAction->setEnabled(true);
    this->restartAction->setEnabled(true);
    this->removeAction->setEnabled(true);
}
