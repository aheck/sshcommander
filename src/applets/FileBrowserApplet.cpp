#include "FileBrowserApplet.h"

FileBrowserApplet::FileBrowserApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->localFileBrowser = new QTreeView(this);
    this->localFileSystemModel = new QFileSystemModel();
    this->localFileSystemModel->setRootPath(QDir::homePath());
    std::cout << "Home dir: " << QDir::homePath().toStdString() << "\n";
    this->localFileBrowser->setModel(this->localFileSystemModel);

    this->remoteFileBrowser = new QTreeView(this);
    this->remoteFileSystemModel = new SFTPFilesystemModel();
    this->remoteFileBrowser->setModel(this->remoteFileSystemModel);

    this->splitter = new QSplitter(this);
    this->splitter->addWidget(this->localFileBrowser);
    this->splitter->addWidget(this->remoteFileBrowser);

    this->layout()->addWidget(this->splitter);
}

const QString FileBrowserApplet::getDisplayName()
{
    return tr("File Browser");
}

QIcon FileBrowserApplet::getIcon()
{
    return QIcon(":/images/drive-harddisk.svg");
}

void FileBrowserApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
}

void FileBrowserApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->remoteFileSystemModel->setConnEntry(connEntry);
}
