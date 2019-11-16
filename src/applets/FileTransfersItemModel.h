/*****************************************************************************
 *
 * FileTransfersItemModel is the model used by FileTransfersApplet to show
 * a table of all file transfers for the current connection as managed by
 * SSHConnectionManager.
 *
 ****************************************************************************/

#ifndef FILETRANSFERSITEMMODEL_H
#define FILETRANSFERSITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>
#include <QUuid>

#include "FileTransferJob.h"
#include "SSHConnectionManager.h"
#include "Util.h"

enum class FileTransferColumns {TransferType = 0, Source, Destination, Transferred, Speed, State, Count};

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

public slots:
    void jobDataChanged(QUuid jobUuid);
    bool removeFileTransferJob(int row);

private:
    QString connectionId;
};

#endif
