/*****************************************************************************
 *
 * InstanceItemModel is the model behind the table showing AWS instances in
 * the AWSWidget.
 *
 * It is supplied with vectors of AWSInstance objects and is responsible for
 * things like sorting and searching of instances.
 *
 ****************************************************************************/

#ifndef INSTANCEITEMMODEL_H
#define INSTANCEITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>

#include "AWS/AWSConnector.h"

enum Column {ID = 0, NAME, STATUS, TYPE, KEYNAME, PUBLICIP, PRIVATEIP, CFSTACK, VPC, LAUNCHTIME, NUM_COLUMNS};

struct InstanceComparator {
    int column;

    bool operator() (const std::shared_ptr<AWSInstance> &a, const std::shared_ptr<AWSInstance> &b);
};

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
    void clear();

    void setInstances(std::vector<std::shared_ptr<AWSInstance>> instances);
    std::shared_ptr<AWSInstance> getInstance(const QModelIndex &index);
    void setSearchTextFilter(const QString searchText);
    void setVpcFilter(const QString vpcId);
    std::vector<std::shared_ptr<AWSInstance>> getInstancesByVpcId(QString vpcId);
    void resolveAllReferences();

private:
    void applyFilters();

    // The list of all the AWSInstance objects that can potentially be shown
    // in the current table.
    std::vector<std::shared_ptr<AWSInstance>> allInstances;

    // The list of AWSInstance objects that are actually visible in the current
    // table. This list can be sorted as well as filtered for a search term.
    std::vector<std::shared_ptr<AWSInstance>> instances;

    QString searchTextFilter;
    QString vpcIdFilter;
};

#endif
