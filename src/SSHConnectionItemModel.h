/*****************************************************************************
 *
 * SSHConnectionItemModel is the model behind the list of connections the
 * user sees on the left-hand side of the main window (ConnectionListView).
 *
 * It stores a list of shared pointers to the SSHConnectionEntry objects of
 * every SSH connection configured by the user.
 *
 ****************************************************************************/

#ifndef SSHCONNECTIONITEMMODEL_H
#define SSHCONNECTIONITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>

#include "AWSCache.h"
#include "SSHConnectionEntry.h"

class SSHConnectionItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void appendConnectionEntry(std::shared_ptr<SSHConnectionEntry> entry);
    void removeConnectionEntry(std::shared_ptr<SSHConnectionEntry> entry);
    std::shared_ptr<SSHConnectionEntry> getConnEntry(int index);
    std::shared_ptr<SSHConnectionEntry> getConnEntryByName(const QString name);
    QModelIndex getIndexForSSHConnectionEntry(const std::shared_ptr<SSHConnectionEntry> entry) const;
    void updateAWSInstances();

private:
    QList<std::shared_ptr<SSHConnectionEntry>> entries;
    QHash<QString, std::shared_ptr<SSHConnectionEntry>> sshConnByHost;
};

#endif
