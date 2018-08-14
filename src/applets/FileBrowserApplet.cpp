#include "FileBrowserApplet.h"

FileBrowserApplet::FileBrowserApplet()
{
    this->firstShow = true;

    this->toolBar = new QToolBar();
    this->toolBar->setOrientation(Qt::Vertical);
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            tr("Reload remote directory"), this, SLOT(reloadData()));
    this->toolBar->addSeparator();
    this->showLocalAction = this->toolBar->addAction(QIcon(":/images/drive-harddisk.svg"),
            tr("Show Local File Browser"), this, SLOT(toggleLocalFileBrowser()));
    this->showLocalAction->setCheckable(true);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->localFileBrowser = new QTreeView();
    this->localFileSystemModel = new FileSystemModel();
    this->localFileSystemModel->setRootPath(QDir::homePath());
    std::cout << "Home dir: " << QDir::homePath().toStdString() << "\n";
    this->localFileBrowser->setModel(this->localFileSystemModel);
    this->localFileBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(this->localFileSystemModel, SIGNAL(fileDownloadRequested(QStringList, QString)),
            this, SLOT(fileDownloadRequested(QStringList, QString)));

    this->localFileBrowserWidget = new QWidget();
    this->localFileBrowserWidget->setLayout(new QVBoxLayout());
    QLabel *localLabel = new QLabel(tr("Local") + ": " + QHostInfo::localHostName());
    this->localFileBrowserWidget->layout()->addWidget(localLabel);
    this->localFileBrowserWidget->layout()->addWidget(this->localFileBrowser);
    this->localFileBrowser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->localFileBrowser->setDragDropMode(QAbstractItemView::DragDrop);
    this->localFileBrowser->setDropIndicatorShown(true);

    this->remoteFileBrowser = new RemoteFileView(this);
    this->remoteFileSystemModel = new SFTPFilesystemModel();
    this->remoteFileBrowser->setModel(this->remoteFileSystemModel);
    this->remoteFileBrowser->setAllColumnsShowFocus(true);
    this->remoteFileBrowser->setColumnWidth(1, 500);
    this->remoteFileBrowser->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->remoteFileBrowser->setDragDropMode(QAbstractItemView::DragDrop);
    this->remoteFileBrowser->setDropIndicatorShown(true);
    this->remoteFileBrowser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    connect(this->remoteFileBrowser, SIGNAL(expanded(QModelIndex)), this, SLOT(expanded(QModelIndex)));
    connect(this->remoteFileSystemModel, SIGNAL(fileUploadRequested(QStringList, QString)),
            this, SLOT(fileUploadRequested(QStringList, QString)));

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

void FileBrowserApplet::setFileTransfersApplet(FileTransfersApplet *applet)
{
    this->fileTransfersApplet = applet;
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

void FileBrowserApplet::fileUploadRequested(QStringList files, QString targetPath)
{
    auto transferJob = std::make_shared<FileTransferJob>(this->connEntry, FileTransferType::Upload, targetPath);
    connect(transferJob.get(), SIGNAL(dataChanged(QUuid)), this->fileTransfersApplet, SLOT(jobDataChanged(QUuid)));

    for (const QString &filename : files) {
        transferJob->addFileToCopy(filename);
    }

    SSHConnectionManager::getInstance().addFileTransferJob(transferJob);

    this->fileTransfersApplet->reloadData();
}

void FileBrowserApplet::fileDownloadRequested(QStringList files, QString targetPath)
{
    auto transferJob = std::make_shared<FileTransferJob>(this->connEntry, FileTransferType::Download, targetPath);
    connect(transferJob.get(), SIGNAL(dataChanged(QUuid)), this->fileTransfersApplet, SLOT(jobDataChanged(QUuid)));

    for (const QString &filename : files) {
        transferJob->addFileToCopy(filename);
    }

    SSHConnectionManager::getInstance().addFileTransferJob(transferJob);

    this->fileTransfersApplet->reloadData();
}
