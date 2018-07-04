#include "FileTransfersApplet.h"

FileTransfersApplet::FileTransfersApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));

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

void FileTransfersApplet::reloadData()
{
    this->model->reloadData();
}
