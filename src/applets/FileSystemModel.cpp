#include "FileSystemModel.h"

FileSystemModel::FileSystemModel()
    : QFileSystemModel()
{

}

FileSystemModel::~FileSystemModel()
{

}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QFileSystemModel::flags(index);

    if (!index.isValid())
        return defaultFlags;

    const QFileInfo& fileInfo = this->fileInfo(index);

    if (fileInfo.isDir()) {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
    else if (fileInfo.isFile()) {
        return Qt::ItemIsDragEnabled | defaultFlags;
    }

    return defaultFlags;
}

bool FileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    QStringList filenames;

    if (action != Qt::CopyAction) {
        return false;
    }

    if (data == nullptr) {
        return false;
    }

    QString targetDir = this->filePath(parent);

    for (QUrl const& url: data->urls()) {
        std::cout << url.toString().toStdString() << "\n";
        filenames.append(url.path());
    }

    emit fileDownloadRequested(filenames, targetDir);

    return true;
}
