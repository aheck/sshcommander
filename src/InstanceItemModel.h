#ifndef INSTANCEITEMMODEL_H
#define INSTANCEITEMMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>

#include "AWSConnector.h"

class InstanceItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setInstances(QVector<AWSInstance*> instances);
    AWSInstance* getInstance(const QModelIndex &index);

private:
    QVector<AWSInstance*> instances;
};

#endif
