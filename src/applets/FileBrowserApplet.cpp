#include "FileBrowserApplet.h"

FileBrowserApplet::FileBrowserApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->addSeparator();
    this->showLocalAction = this->toolBar->addAction(QIcon(":/images/drive-harddisk.svg"),
            tr("Show Local File Browser"), this, SLOT(toggleLocalFileBrowser()));
    this->showLocalAction->setCheckable(true);
    this->toolBar->addAction(QIcon(":/images/drive-harddisk.svg"),
            tr("Start Download"), this, SLOT(startDownload()));
    this->toolBar->addAction(QIcon(":/images/drive-harddisk.svg"),
            tr("Start Upload"), this, SLOT(startUpload()));

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->localFileBrowser = new QTreeView();
    this->localFileSystemModel = new QFileSystemModel();
    this->localFileSystemModel->setRootPath(QDir::homePath());
    std::cout << "Home dir: " << QDir::homePath().toStdString() << "\n";
    this->localFileBrowser->setModel(this->localFileSystemModel);
    this->localFileBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->localFileBrowserWidget = new QWidget();
    this->localFileBrowserWidget->setLayout(new QVBoxLayout());
    QLabel *localLabel = new QLabel(tr("Local") + ": " + QHostInfo::localHostName());
    this->localFileBrowserWidget->layout()->addWidget(localLabel);
    this->localFileBrowserWidget->layout()->addWidget(this->localFileBrowser);
    this->localFileBrowser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    this->remoteFileBrowser = new QTreeView(this);
    this->remoteFileSystemModel = new SFTPFilesystemModel();
    this->remoteFileBrowser->setModel(this->remoteFileSystemModel);
    this->remoteFileBrowser->setAllColumnsShowFocus(true);
    this->remoteFileBrowser->setColumnWidth(1, 500);
    this->remoteFileBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->remoteFileBrowser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(this->remoteFileBrowser, SIGNAL(expanded(QModelIndex)), this, SLOT(expanded(QModelIndex)));

    QWidget *remoteFileBrowserWidget = new QWidget();
    remoteFileBrowserWidget->setLayout(new QVBoxLayout());
    this->remoteHostnameLabel = new QLabel(tr("Remote"));
    remoteFileBrowserWidget->layout()->addWidget(this->remoteHostnameLabel);
    remoteFileBrowserWidget->layout()->addWidget(this->remoteFileBrowser);

    this->splitter = new QSplitter(this);
    this->splitter->addWidget(remoteFileBrowserWidget);

    this->layout()->addWidget(this->splitter);
}

FileBrowserApplet::~FileBrowserApplet()
{
    if (this->localFileBrowserWidget->parentWidget() == nullptr) {
        delete this->localFileBrowserWidget;
    }
}

const QString FileBrowserApplet::getDisplayName()
{
    return tr("File Browser");
}

QIcon FileBrowserApplet::getIcon()
{
    return QIcon(":/images/folder-open.svg");
}

void FileBrowserApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    this->remoteHostnameLabel->setText(tr("Remote") + ": " + connEntry->hostname);
}

void FileBrowserApplet::onShow()
{
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->remoteFileSystemModel->setConnEntry(connEntry);
}

void FileBrowserApplet::expanded(const QModelIndex &index)
{
    this->lastIndexExpanded = index;
}

void FileBrowserApplet::reloadData()
{
    QString path;

    if (!this->lastIndexExpanded.isValid()) {
        return;
    } else {
        QString *pathPtr = static_cast<QString*>(this->lastIndexExpanded.internalPointer());
        if (pathPtr == nullptr) {
            return;
        }

        path = *pathPtr;
    }

    int numRowsBefore = this->remoteFileSystemModel->rowCount(this->lastIndexExpanded);

    int numRowsAfter = this->remoteFileSystemModel->loadDirectory(path);
    this->remoteFileSystemModel->sendReloadNotification(this->lastIndexExpanded, numRowsBefore, numRowsAfter);
}

void FileBrowserApplet::toggleLocalFileBrowser()
{
    if (!this->showLocalAction->isChecked()) {
        this->showLocalAction->setToolTip(tr("Show Local File Browser"));
        this->localFileBrowserWidget->setParent(nullptr);
    } else {
        this->showLocalAction->setToolTip(tr("Hide Local File Browser"));
        this->splitter->insertWidget(0, this->localFileBrowserWidget);
    }
}

void FileBrowserApplet::startDownload()
{
    auto transferJob = std::make_shared<FileTransferJob>(this->connEntry, FileTransferType::Download, "/tmp");
    transferJob->addFileToCopy("/home/ahe/download_test_file");

    SSHConnectionManager::getInstance().addFileTransferJob(transferJob);
}

void FileBrowserApplet::startUpload()
{
    auto transferJob = std::make_shared<FileTransferJob>(this->connEntry, FileTransferType::Upload, "/tmp");
    transferJob->addFileToCopy("/home/aheck/SSHConnectionManager.h");

    SSHConnectionManager::getInstance().addFileTransferJob(transferJob);
}
