/*****************************************************************************
 *
 * SSHFilesystemModel is the model behind the SFTP file browser.
 *
 ****************************************************************************/

#ifndef SFTPFILESYSTEMMODEL_H
#define SFTPFILESYSTEMMODEL_H

#include <map>
#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QIcon>
#include <QModelIndex>

#include "SSHConnectionManager.h"

enum class SFTPColumns {Name = 0, Size, Type, Modified, Permissions, User, Count};

class SFTPFilesystemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    SFTPFilesystemModel();
    ~SFTPFilesystemModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void clear();

    void updateData(QString data);
    void sort(int column, Qt::SortOrder order) override;

    void setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);
    void reloadData();
    void setShowOnlyDirs(bool showOnlyDirs);

private:
    void addPathString(QString pathString);
    QString* getPathString(QString pathString) const;
    QString dirname(QString path) const;
    QString basename(QString path) const;
    QString formatBytes(uint64_t numBytes) const;
    double roundBytesUp(double numBytes) const;

    // for debugging
    void dumpPathStrings() const;

    bool showOnlyDirs;
    std::shared_ptr<SSHConnectionEntry> connEntry;

    // maps a path string to a string pointer which we can use as
    // the internalPointer value of a QModelIndex
    std::map<QString, QString*> pathStrings;

    // keeps a vector of directory entries for each path we already
    // retrieved via SFTP
    std::map<QString, std::vector<std::shared_ptr<DirEntry>>> dirCache;

    // lookup table to find out if a path is a directory
    std::map<QString, bool> pathIsDirTable;
};

#endif
