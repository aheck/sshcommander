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
        case Column::VPC:
            return QVariant("Virtual Private Cloud");
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
                return QIcon(":/images/green-light.svg");
            } else if (instance->status == "terminated") {
                return QIcon(":/images/red-light.svg");
            }

            return QIcon(":/images/yellow-light.svg");
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
        case Column::VPC:
            return QVariant(instance->formattedVpc());
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

    this->applyFilters();

    this->endResetModel();
}

void InstanceItemModel::setSearchTextFilter(const QString searchText)
{
    this->searchTextFilter = searchText;

    this->applyFilters();
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
        case Column::VPC:
            return a->formattedVpc().compare(b->formattedVpc()) < 0;
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

void InstanceItemModel::clear()
{
    emit layoutAboutToBeChanged();

    this->instances.clear();
    this->allInstances.clear();

    emit layoutChanged();
}

std::vector<std::shared_ptr<AWSInstance>> InstanceItemModel::getInstancesByVpcId(QString vpcId)
{
    std::vector<std::shared_ptr<AWSInstance>> result;

    for (auto instance : this->allInstances) {
        if (instance->vpcId == vpcId && instance->status == "running") {
            result.push_back(instance);
        }
    }

    return result;
}

void InstanceItemModel::setVpcFilter(const QString vpcId)
{
    this->vpcIdFilter = vpcId;

    this->applyFilters();
}

void InstanceItemModel::applyFilters()
{
    this->beginResetModel();

    if (this->searchTextFilter.isEmpty() && this->vpcIdFilter.isEmpty()) {
        this->instances = this->allInstances;
    } else {
        this->instances.clear();

        for (std::shared_ptr<AWSInstance> instance : this->allInstances) {
            if (!this->vpcIdFilter.isEmpty()) {
                if (instance->vpcId != this->vpcIdFilter) {
                    continue;
                }
            }

            // check the instance tags for a match (Name is also a tag so no
            // need to check it separately)
            bool found = false;
            for (AWSTag tag : instance->tags) {
                if (tag.key.startsWith(this->searchTextFilter, Qt::CaseInsensitive) ||
                        tag.value.startsWith(this->searchTextFilter, Qt::CaseInsensitive)) {
                    this->instances.push_back(instance);
                    found = true;
                    break;
                }
            }

            // if we have found a match in the tags for-loop we continue with
            // the next instance
            if (found) {
                continue;
            }

            // check other important fields for a match
            if (instance->id.startsWith(this->searchTextFilter, Qt::CaseInsensitive)) {
                this->instances.push_back(instance);
                continue;
            }

            if (instance->keyname.startsWith(this->searchTextFilter, Qt::CaseInsensitive)) {
                this->instances.push_back(instance);
                continue;
            }

            if (instance->publicIP.startsWith(this->searchTextFilter, Qt::CaseInsensitive)) {
                this->instances.push_back(instance);
                continue;
            }

            if (instance->privateIP.startsWith(this->searchTextFilter, Qt::CaseInsensitive)) {
                this->instances.push_back(instance);
                continue;
            }
        }
    }

    this->endResetModel();
}

void InstanceItemModel::resolveAllReferences()
{
    for (auto instance : this->allInstances) {
        instance->resolveReferences();
    }
}
