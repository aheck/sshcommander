#ifndef FILETRANSFERSITEMMODEL_H
#define FILETRANSFERSITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>

#include "SSHConnectionManager.h"

enum class FileTransferColumns {TransferType = 0, Source, Destination, Transferred, Speed, Count};

class FileTransfersItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;

    void setConnectionId(const QString &connectionId);
    void reloadData();

private:
    QString connectionId;
};

#endif
