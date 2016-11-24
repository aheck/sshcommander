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
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void setInstances(QVector<AWSInstance*> instances);
    AWSInstance* getInstance(const QModelIndex &index);

private:
    QVector<AWSInstance*> instances;
};

#endif
