#include "InstanceItemModel.h"

enum Column {ID = 0, NAME, STATUS, TYPE, KEYNAME, PUBLICIP, PRIVATEIP, CFSTACK, LAUNCHTIME, NUM_COLUMNS};

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
    return Column::NUM_COLUMNS;
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
        case Column::ID:
            return QVariant("Instance ID");
        case Column::NAME:
            return QVariant("Name");
        case Column::STATUS:
            return QVariant("Status");
        case Column::TYPE:
            return QVariant("Type");
        case Column::KEYNAME:
            return QVariant("SSH Key");
        case Column::PUBLICIP:
            return QVariant("Public IP");
        case Column::PRIVATEIP:
            return QVariant("Private IP");
        case Column::CFSTACK:
            return QVariant("CloudFormation Stack");
        case Column::LAUNCHTIME:
            return QVariant("Launch Time");
    }

    return QVariant();
}

QVariant InstanceItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    std::shared_ptr<AWSInstance> instance = this->instances.at(index.row());

    if (role== Qt::DecorationRole) {
        if (index.column() == 2) {
            if (instance->status == "running") {
                return QColor(Qt::green);
            } else if (instance->status == "terminated") {
                return QColor(Qt::red);
            }

            return QColor(Qt::yellow);
        }

        return QVariant();
    }

    switch (index.column()) {
        case Column::ID:
            return QVariant(instance->id);
        case Column::NAME:
            return QVariant(instance->name);
        case Column::STATUS:
            return QVariant(instance->status);
        case Column::TYPE:
            return QVariant(instance->type);
        case Column::KEYNAME:
            return QVariant(instance->keyname);
        case Column::PUBLICIP:
            return QVariant(instance->publicIP);
        case Column::PRIVATEIP:
            return QVariant(instance->privateIP);
        case Column::CFSTACK:
            return QVariant(instance->cfStackName);
        case Column::LAUNCHTIME:
            return QVariant(instance->launchTime);
    }

    return QVariant();
}

void InstanceItemModel::setInstances(std::vector<std::shared_ptr<AWSInstance>> instances)
{
    this->beginResetModel();

    this->instances.clear();
    this->allInstances = instances;
    this->instances = this->allInstances;

    this->endResetModel();
}

void InstanceItemModel::setSearchText(const QString searchText)
{
    this->beginResetModel();

    if (searchText.isEmpty()) {
        this->instances = this->allInstances;
    } else {
        this->instances.clear();

        for (std::shared_ptr<AWSInstance> instance : this->allInstances) {
            // check the instance name for a match
            if (instance->name.startsWith(searchText, Qt::CaseInsensitive)) {
                this->instances.push_back(instance);
                continue;
            }

            // check the instance tags for a match
            for (AWSTag tag : instance->tags) {
                if (tag.key.startsWith(searchText, Qt::CaseInsensitive) ||
                        tag.value.startsWith(searchText, Qt::CaseInsensitive)) {
                    this->instances.push_back(instance);
                    break;
                }
            }
        }
    }

    this->endResetModel();
}

std::shared_ptr<AWSInstance> InstanceItemModel::getInstance(const QModelIndex &index)
{
    if (index.row() > (this->instances.size() - 1)) {
        return nullptr;
    }

    return this->instances.at(index.row());
}

bool InstanceComparator::operator() (const std::shared_ptr<AWSInstance> &a, const std::shared_ptr<AWSInstance> &b) {
    bool result = false;

    if (a.get() == b.get()) {
        return false;
    }

    switch (this->column) {
        case Column::ID:
            result = a->id.compare(b->id) < 0;
            break;
        case Column::NAME:
            result = a->name.compare(b->name) < 0;
            break;
        case Column::STATUS:
            result = a->status.compare(b->status) < 0;
            break;
        case Column::TYPE:
            result = a->type.compare(b->type) < 0;
            break;
        case Column::KEYNAME:
            result = a->keyname.compare(b->keyname) < 0;
            break;
        case Column::PUBLICIP:
            result = a->publicIP.compare(b->publicIP) < 0;
            break;
        case Column::PRIVATEIP:
            result = a->privateIP.compare(b->privateIP) < 0;
            break;
        case Column::CFSTACK:
            result = a->cfStackName.compare(b->cfStackName) < 0;
            break;
        case Column::LAUNCHTIME:
            result = a->launchTime.compare(b->launchTime) < 0;
            break;
    }

    return result;
}

void InstanceItemModel::sort(int column, Qt::SortOrder order)
{
    InstanceComparator cmp;

    emit layoutAboutToBeChanged();

    cmp.column = column;

    if (order == Qt::DescendingOrder) {
        std::stable_sort(this->instances.rbegin(), this->instances.rend(), cmp);
    } else {
        std::stable_sort(this->instances.begin(), this->instances.end(), cmp);
    }

    emit layoutChanged();
}
