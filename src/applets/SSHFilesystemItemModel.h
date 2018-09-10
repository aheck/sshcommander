/*****************************************************************************
 *
 * SSHFilesystemItemModel is the model behind the table showing SSHFS mounts
 * on the local workstation.
 *
 ****************************************************************************/

#ifndef SSHFILESYSTEMITEMMODEL_H
#define SSHFILESYSTEMITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>

#include "SSHFilesystemManager.h"

enum class SSHFSColumns {LocalDir = 0, RemoteDir, ShortDescription, Mounted, Count};

class SSHFilesystemItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void clear();

    void updateData(QString data);
    void sort(int column, Qt::SortOrder order) override;

    // In SSHFilesystemManager each mount is identified by a string like "user@host"
    void setConnectionStrings(QString username, QString hostname);
    void reloadData();

private:
    QString username;
    QString hostname;
};

#endif
