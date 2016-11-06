#include "InstanceItemModel.h"

QModelIndex InstanceItemModel::index(int row, int column, const QModelIndex &parent = QModelIndex()) const
{
    return this->createIndex(row, column);
}

QModelIndex InstanceItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int InstanceItemModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    return this->instances.size();
}

int InstanceItemModel::columnCount(const QModelIndex &parent = QModelIndex()) const
{
    return 7;
}

QVariant InstanceItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case 0:
            return QVariant("ID");
        case 1:
            return QVariant("Status");
        case 2:
            return QVariant("Type");
        case 3:
            return QVariant("SSH Key");
        case 4:
            return QVariant("Public IP");
        case 5:
            return QVariant("Private IP");
        case 6:
            return QVariant("Launch Time");
    }

    return QVariant();
}

QVariant InstanceItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    AWSInstance *instance = this->instances.at(index.row());

    switch (index.column()) {
        case 0:
            return QVariant(instance->id);
        case 1:
            return QVariant(instance->status);
        case 2:
            return QVariant(instance->type);
        case 3:
            return QVariant(instance->keyname);
        case 4:
            return QVariant(instance->publicIP);
        case 5:
            return QVariant(instance->privateIP);
        case 6:
            return QVariant(instance->launchTime);
    }

    return QVariant();
}

void InstanceItemModel::setInstances(QVector<AWSInstance*> instances)
{
    this->beginResetModel();
    this->instances = instances;
    this->endResetModel();
}
