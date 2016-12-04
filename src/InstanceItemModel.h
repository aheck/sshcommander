#ifndef INSTANCEITEMMODEL_H
#define INSTANCEITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
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

    void setInstances(QVector<std::shared_ptr<AWSInstance>> instances);
    std::shared_ptr<AWSInstance> getInstance(const QModelIndex &index);
    void setSearchText(const QString searchText);

private:
    // The list of all the AWSInstance objects that can potentially be shown
    // in the current table.
    QVector<std::shared_ptr<AWSInstance>> allInstances;

    // The list of AWSInstance objects that are actually visible in the current
    // table. This list can be sorted as well as filtered for a search term.
    QVector<std::shared_ptr<AWSInstance>> instances;
};

#endif
