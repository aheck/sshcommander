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

bool FileSystemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (data == nullptr) {
        return false;
    }

    if (data->hasFormat("application/sftp-files")) {
        return true;
    }

    return false;
}

bool FileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action != Qt::CopyAction) {
        return false;
    }

    if (data == nullptr) {
        return false;
    }

    if (!data->hasFormat("application/sftp-files")) {
        return false;
    }

    QString targetDir = this->filePath(parent);
    QStringList filenames = QString::fromUtf8(data->data("application/sftp-files")).split("\n");

    emit fileDownloadRequested(filenames, targetDir);

    return true;
}
