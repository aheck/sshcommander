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
        std::cerr << "index parent path: " << parentPath->toStdString() << "\n";

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

        std::cerr << "index pathStr: " << pathStr.toStdString() << "\n";
        std::cerr << "index str: " << str << "\n";
        std::cerr << "index str: " << str->toStdString() << "\n";
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

    std::cerr << "parent: path: " << path->toStdString() << "\n";

    QString parentPath = this->dirname(*path);

    std::cerr << "parent: parentPath: " << parentPath.toStdString() << "\n";

    if (parentPath == "/") {
        row = 0;
    } else {
        if (this->dirCache.count(parentPath) != 1) {
            return QModelIndex();
        }

        auto dirEntries = this->dirCache.at(parentPath);

        int i = 0;
        for (auto dirEntry : dirEntries) {
            if (dirEntry->getFilename() == this->basename(*path)) {
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
        std::cerr << "dirPath: " << dirPath.toStdString() << "\n";
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

    std::cout << "Calling readDirectory for " << parentPath->toStdString() << "\n";
    auto entries = SSHConnectionManager::getInstance().readDirectory(this->connEntry, *parentPath, this->showOnlyDirs);
    std::sort(entries.begin(), entries.end(), compareDirEntries);
    this->dirCache[*parentPath] = entries;

    for (auto const &dirEntry : entries) {
        if (dirEntry->getPath() == "/") {
            this->addPathString(dirEntry->getPath() + dirEntry->getFilename());
            this->pathIsDirTable[dirEntry->getPath() + dirEntry->getFilename()] = dirEntry->isDirectory();
        } else {
            this->addPathString(dirEntry->getPath() + "/" + dirEntry->getFilename());
            this->pathIsDirTable[dirEntry->getPath() + "/" + dirEntry->getFilename()] = dirEntry->isDirectory();
        }
    }

    std::cout << "Finished readDirectory for " << parentPath->toStdString() << "\n";
    this->dumpPathStrings();
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

    QString parentPath = this->dirname(*path);

    if (this->dirCache.count(parentPath) != 1) {
        std::cerr << "dirEntries has no entry for " << parentPath.toStdString() << "\n";
        return QVariant();
    }

    auto dirEntries = this->dirCache.at(parentPath);
    std::cerr << "dirEntries size: " << dirEntries.size() << "\n";
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
            return QVariant(this->formatBytes(dirEntry->getFilesize()));
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

void SFTPFilesystemModel::updateData(QString data)
{
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

void SFTPFilesystemModel::reloadData()
{
    this->beginResetModel();
    this->endResetModel();
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

QString SFTPFilesystemModel::dirname(QString path) const
{
    while (path.endsWith("/") && path != "/") {
        path.chop(1);
    }

    if (path == "/") {
        return path;
    }

    int pos = path.lastIndexOf("/");
    if (pos == -1) {
        return ".";
    }

    path.truncate(pos);
    if (path.length() == 0) {
        path = "/";
    }
    return path;
}

QString SFTPFilesystemModel::basename(QString path) const
{
    while (path.endsWith("/") && path != "/") {
        path.chop(1);
    }

    if (path == "/") {
        return path;
    }

    int pos = path.lastIndexOf("/");
    if (pos == -1) {
        return path;
    }

    return path.remove(0, pos + 1);
}

QString SFTPFilesystemModel::formatBytes(uint64_t numBytes) const
{
    double gb = 1024 * 1024 * 1024;
    double mb = 1024 * 1024;
    double kb = 1024;
    double result;

    if (numBytes >= gb) {
        result = this->roundBytesUp(numBytes / gb);
        return QString::number(result, 'g', 1) + " GB";
    }

    if (numBytes >= mb) {
        result = this->roundBytesUp(numBytes / mb);
        return QString::number(result, 'g', 1) + " MB";
    }

    if (numBytes >= kb) {
        result = this->roundBytesUp(numBytes / kb);
        return QString::number(result, 'f', 1) + " KB";
    }

    return QString::number(numBytes) + " bytes";
}

double SFTPFilesystemModel::roundBytesUp(double numBytes) const
{
    // we display the byte value to the first position after the decimal point
    // and we always want to round the bytes to the next higher value
    return numBytes + 0.04;
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
