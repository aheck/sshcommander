/*****************************************************************************
 *
 * FileSystemModel is a specialized version of QFileSystemModel adding
 * support for starting file transfers by drag and drop.
 *
 ****************************************************************************/

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <iostream>

#include <QFileSystemModel>
#include <QMimeData>
#include <QString>
#include <QStringList>
#include <QUrl>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    FileSystemModel();
    ~FileSystemModel();

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

signals:
    void fileDownloadRequested(QStringList files, QString targetPath);

};

#endif
