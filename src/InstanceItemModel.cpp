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
    return 8;
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
            return QVariant("Name");
        case 2:
            return QVariant("Status");
        case 3:
            return QVariant("Type");
        case 4:
            return QVariant("SSH Key");
        case 5:
            return QVariant("Public IP");
        case 6:
            return QVariant("Private IP");
        case 7:
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
            return QVariant(instance->name);
        case 2:
            return QVariant(instance->status);
        case 3:
            return QVariant(instance->type);
        case 4:
            return QVariant(instance->keyname);
        case 5:
            return QVariant(instance->publicIP);
        case 6:
            return QVariant(instance->privateIP);
        case 7:
            return QVariant(instance->launchTime);
    }

    return QVariant();
}

void InstanceItemModel::setInstances(QVector<AWSInstance*> instances)
{
    this->beginResetModel();

    // free the memory of the old instance objects before we lose access to
    // their pointers
    for (int i = 0; i < this->instances.size(); i++) {
        delete this->instances.at(i);
    }

    this->instances = instances;

    this->endResetModel();
}

AWSInstance* InstanceItemModel::getInstance(const QModelIndex &index)
{
    if (index.row() > (this->instances.size() - 1)) {
        return nullptr;
    }

    return this->instances.at(index.row());
}

struct Comparator {
    int column;
    Qt::SortOrder order;

    bool operator() (AWSInstance *a, AWSInstance *b) {
        bool result = false;

        switch (this->column) {
            case 0:
                result = a->id.compare(b->id) < 0;
                break;
            case 1:
                result = a->name.compare(b->name) < 0;
                break;
            case 2:
                result = a->status.compare(b->status) < 0;
                break;
            case 3:
                result = a->type.compare(b->type) < 0;
                break;
            case 4:
                result = a->keyname.compare(b->keyname) < 0;
                break;
            case 5:
                result = a->publicIP.compare(b->publicIP) < 0;
                break;
            case 6:
                result = a->privateIP.compare(b->privateIP) < 0;
                break;
            case 7:
                result = a->launchTime.compare(b->launchTime) < 0;
                break;
        }

        if (this->order == Qt::AscendingOrder) {
            return !result;
        }

        return result;
    }
};

void InstanceItemModel::sort(int column, Qt::SortOrder order)
{
    Comparator cmp;

    emit layoutAboutToBeChanged();

    cmp.column = column;
    cmp.order = order;
    std::sort(this->instances.begin(), this->instances.end(), cmp);

    emit layoutChanged();
}
