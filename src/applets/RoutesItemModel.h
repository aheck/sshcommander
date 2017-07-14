/*****************************************************************************
 *
 * RoutesItemModel is the model behind the table showing routes on remote
 * machines.
 *
 ****************************************************************************/

#ifndef ROUTESITEMMODEL_H
#define ROUTESITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

enum class RouteColumns {Destination = 0, Gateway, Genmask, Interface, Count};

struct RouteEntry {
    QString destination;
    QString gateway;
    QString genmask;
    QString interface;
};

struct RouteEntryComparator {
    int column;

    bool operator() (const std::shared_ptr<RouteEntry> &a, const std::shared_ptr<RouteEntry> &b);
};

class RoutesItemModel : public QAbstractItemModel
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
    void sort(int column, Qt::SortOrder order) override;

    void updateData(QString data);

private:
    std::vector<std::shared_ptr<RouteEntry>> routesData;
};

#endif
