#include "SFTPFilesystemModel.h"

bool compareDirEntries(std::shared_ptr<DirEntry> dirEntry1, std::shared_ptr<DirEntry> dirEntry2);

SFTPFilesystemModel::SFTPFilesystemModel():
    QAbstractItemModel()
{
    this->showOnlyDirs = false;
    this->addPathString(QString("/"));
}

SFTPFilesystemModel::~SFTPFilesystemModel()
{
    for (const auto &kv : this->pathStrings) {
        delete kv.second;
    }
}

QModelIndex SFTPFilesystemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= columnCount(parent)) {// || row >= rowCount(parent) ) {
        return QModelIndex();
    }

    QString *str = nullptr;

    if (!parent.isValid()) {
        str = this->getPathString("/");
    } else {
        QString *parentPath = static_cast<QString*>(parent.internalPointer());

        if (this->dirCache.count(*parentPath) != 1) {
            return QModelIndex();
        }

        auto dirEntries = this->dirCache.at(*parentPath);
        auto dirEntry = dirEntries.at(row);
        QString pathStr = *parentPath;

        if (pathStr != "/") {
            pathStr += "/";
        }
        pathStr += dirEntry->getFilename();
        str = this->getPathString(pathStr);
    }

    return this->createIndex(row, column, static_cast<void*>(str));
}

QModelIndex SFTPFilesystemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    if (index.column() != 0) {
        return QModelIndex();
    }

    int row = -1;
    QString *path = static_cast<QString*>(index.internalPointer());
    if (path == nullptr) {
        std::cerr << "SFTPFilesystemModel::parent: internalPointer is NULL but expected a valid pointer\n";
        return QModelIndex();
    }

    if (*path == "/") {
        return QModelIndex();
    }

    QString parentPath = Util::dirname(*path);

    if (parentPath == "/") {
        row = 0;
    } else {
        if (this->dirCache.count(parentPath) != 1) {
            return QModelIndex();
        }

        auto dirEntries = this->dirCache.at(parentPath);

        int i = 0;
        for (auto dirEntry : dirEntries) {
            if (dirEntry->getFilename() == Util::basename(*path)) {
                row = i;
                break;
            }

            i++;
        }

        if (row == -1) {
            return QModelIndex();
        }
    }

    QString *pathStr = this->getPathString(parentPath);
    return this->createIndex(row, 0, static_cast<void*>(pathStr));
}

int SFTPFilesystemModel::rowCount(const QModelIndex &parent) const
{
    if (this->connEntry == nullptr) {
        return 0;
    }

    if (!parent.isValid()) {
        return 1;
    }

    QString *parentPath = static_cast<QString*>(parent.internalPointer());
    std::cerr << "rowCount parentPath: " << parentPath->toStdString() << "\n";

    if (this->dirCache.count(*parentPath) == 1) {
        try {
            return this->dirCache.at(*parentPath).size();
        } catch (std::out_of_range) {
            return 0;
        }
    }

    return 0;
}

int SFTPFilesystemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(SFTPColumns::Count);
}

Qt::ItemFlags SFTPFilesystemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }

    return QAbstractItemModel::flags(index);
}

bool SFTPFilesystemModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return true;
    }

    QString *path = static_cast<QString*>(parent.internalPointer());
    if (path == nullptr) {
        return false;
    }

    if (*path == "/") {
        return true;
    }

    return this->pathIsDirTable.at(*path);
}

bool SFTPFilesystemModel::canFetchMore(const QModelIndex &parent) const
{
    QString *path = static_cast<QString*>(parent.internalPointer());
    if (path == nullptr) {
        std::cerr << "SFTPFilesystemModel: internalPointer is NULL but expected a valid pointer\n";
        return false;
    }

    QString pathToCheck = *path;

    if (*path == ".") {
        pathToCheck = "/";
    }

    if (this->dirCache.count(pathToCheck) == 1) {
        return false;
    }

    return true;
}

void SFTPFilesystemModel::fetchMore(const QModelIndex &parent)
{
    if (this->connEntry == nullptr) {
        return;
    }

    QString *parentPath = static_cast<QString*>(parent.internalPointer());
    if (parentPath == nullptr) {
        return;
    }

    // is this directory already in the cache?
    if (this->dirCache.count(*parentPath) == 1) {
        return;
    }

    // check if parentPath is a directory
    if (*parentPath != "/") {
        QString str = *parentPath;
        str.remove(0, 1); // remove leading /
        QStringList components = str.split("/");
        QString filename = components.last();
        components.removeLast();
        QString dirPath = "/" + components.join("/");

        bool isDir = false;
        auto entries = this->dirCache.at(dirPath);
        for (auto cur : entries) {
            if (cur->getFilename() == filename) {
                isDir = cur->isDirectory();
            }
        }

        if (!isDir) {
            return;
        }
    }

    this->loadDirectory(*parentPath);
}

int SFTPFilesystemModel::loadDirectory(QString path)
{
    std::cout << "Calling readDirectory for " << path.toStdString() << "\n";
    auto entries = SSHConnectionManager::getInstance().readDirectory(this->connEntry, path, this->showOnlyDirs);
    std::sort(entries.begin(), entries.end(), compareDirEntries);
    this->dirCache[path] = entries;

    for (auto const &dirEntry : entries) {
        std::cerr << "loaded path: " << dirEntry->getPath().toStdString() << "\n";
        if (dirEntry->getPath() == "/") {
            this->addPathString(dirEntry->getPath() + dirEntry->getFilename());
            this->pathIsDirTable[dirEntry->getPath() + dirEntry->getFilename()] = dirEntry->isDirectory();
        } else {
            this->addPathString(dirEntry->getPath() + "/" + dirEntry->getFilename());
            this->pathIsDirTable[dirEntry->getPath() + "/" + dirEntry->getFilename()] = dirEntry->isDirectory();
        }
    }

    std::cout << "Finished readDirectory for " << path.toStdString() << "\n";

    return entries.size();
}

QVariant SFTPFilesystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(SFTPColumns::Name):
            return QVariant(tr("Name"));
        case static_cast<int>(SFTPColumns::Size):
            return QVariant(tr("Size"));
        case static_cast<int>(SFTPColumns::Type):
                return QVariant(tr("Type"));
        case static_cast<int>(SFTPColumns::Modified):
            return QVariant(tr("Date Modified"));
        case static_cast<int>(SFTPColumns::Permissions):
            return QVariant(tr("Permissions"));
        case static_cast<int>(SFTPColumns::User):
            return QVariant(tr("User"));
        case static_cast<int>(SFTPColumns::Group):
            return QVariant(tr("Group"));
    }

    return QVariant();
}

QVariant SFTPFilesystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        std::cerr << "\nData called with invalid index!!!!\n\n";
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    QString *path = static_cast<QString*>(index.internalPointer());

    if (*path == "/") {
        if (index.column() != 0) {
            return QVariant();
        }

        if (role == Qt::DisplayRole) {
            return QVariant("/");
        } else {
            QFileIconProvider iconProvider;
            return QVariant(iconProvider.icon(QFileIconProvider::Folder));
        }
    }

    QString parentPath = Util::dirname(*path);

    if (this->dirCache.count(parentPath) != 1) {
        std::cerr << "dirEntries has no entry for " << parentPath.toStdString() << "\n";
        return QVariant();
    }

    auto dirEntries = this->dirCache.at(parentPath);
    auto dirEntry = dirEntries.at(index.row());

    QFileIconProvider iconProvider;
    QDateTime dateTime;

    switch (index.column()) {
        case (static_cast<int>(SFTPColumns::Name)):
            if (role == Qt::DisplayRole) return dirEntry->getFilename();

            if (dirEntry->isDirectory()) {
                return QVariant(iconProvider.icon(QFileIconProvider::Folder));
            }

            return QVariant(iconProvider.icon(QFileIconProvider::File));
        case (static_cast<int>(SFTPColumns::Size)):
            if (dirEntry->isDirectory()) {
                return QVariant();
            }
            return QVariant(Util::formatBytes(dirEntry->getFilesize()));
        case (static_cast<int>(SFTPColumns::Type)):
            if (dirEntry->isDirectory()) {
                return QVariant("Folder");
            } else if (dirEntry->isRegularFile()) {
                return QVariant("File");
            } else if (dirEntry->isSymLink()) {
                return QVariant("Symlink");
            } else if (dirEntry->isCharacterDevice()) {
                return QVariant("Character Device");
            } else if (dirEntry->isBlockDevice()) {
                return QVariant("Block Device");
            } else if (dirEntry->isFIFO()) {
                return QVariant("FIFO");
            } else if (dirEntry->isSocket()) {
                return QVariant("Unix Socket");
            }
            return QVariant();
        case (static_cast<int>(SFTPColumns::Modified)):
            if (!dirEntry->hasMtime()) {
                return QVariant();
            }

            dateTime.setTime_t(dirEntry->getMtime());

            return dateTime;
        case (static_cast<int>(SFTPColumns::Permissions)):
            return dirEntry->getPermissionsString();
        case (static_cast<int>(SFTPColumns::User)):
            return dirEntry->getUser();
        case (static_cast<int>(SFTPColumns::Group)):
            return dirEntry->getGroup();
    }

    return QVariant();
}

void SFTPFilesystemModel::clear()
{
    emit layoutAboutToBeChanged();

    emit layoutChanged();
}

void SFTPFilesystemModel::sort(int column, Qt::SortOrder order)
{
}

void SFTPFilesystemModel::setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    beginResetModel();

    this->connEntry = connEntry;

    endResetModel();
}

void SFTPFilesystemModel::sendReloadNotification(const QModelIndex &parent, int numRowsBefore, int numRowsAfter)
{
    if (numRowsBefore > 0) {
        this->beginRemoveRows(parent, 0, numRowsBefore);
        this->endRemoveRows();
    }

    if (numRowsAfter > 0) {
        this->beginInsertRows(parent, 0, numRowsAfter);
        this->endInsertRows();
    }
}

void SFTPFilesystemModel::addPathString(QString pathString)
{
    if (this->pathStrings.count(pathString) != 0) {
        return;
    }

    this->pathStrings[pathString] = new QString(pathString);
}

QString* SFTPFilesystemModel::getPathString(QString pathString) const
{
    if (this->pathStrings.count(pathString) == 1) {
        return this->pathStrings.at(pathString);
    }

    return nullptr;
}

void SFTPFilesystemModel::dumpPathStrings() const
{
    std::cerr << "Path String Dump:" << "\n";
    for (auto pair : this->pathStrings) {
        std::cerr << pair.second->toStdString() << "\n";
    }
}

void SFTPFilesystemModel::setShowOnlyDirs(bool showOnlyDirs)
{
    this->showOnlyDirs = showOnlyDirs;
}

bool compareDirEntries(std::shared_ptr<DirEntry> dirEntry1, std::shared_ptr<DirEntry> dirEntry2)
{
    // directories are always placed before files
    if (dirEntry1->isDirectory() && !dirEntry2->isDirectory()) {
        return true;
    }

    if (!dirEntry1->isDirectory() && dirEntry2->isDirectory()) {
        return false;
    }

    return dirEntry1->getFilename() < dirEntry2->getFilename();
}
