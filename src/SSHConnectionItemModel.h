#include <QAbstractItemModel>
#include <QModelIndex>
#include <QList>

#include "SSHConnectionEntry.h"

#ifndef SSHCONNECTIONITEMMODEL_H
#define SSHCONNECTIONITEMMODEL_H

class SSHConnectionItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void appendConnectionEntry(SSHConnectionEntry *entry);
    void removeConnectionEntry(SSHConnectionEntry *entry);
    SSHConnectionEntry* getConnEntryByName(const QString name);

private:
    QList<SSHConnectionEntry*> entries;
    QHash<QString, SSHConnectionEntry*> sshConnByHost;
};

#endif
