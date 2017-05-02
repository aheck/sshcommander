#include "ProcessesItemModel.h"

QModelIndex ProcessesItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->createIndex(row, column);
}

QModelIndex ProcessesItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ProcessesItemModel::rowCount(const QModelIndex &parent) const
{
    return this->procData.size();
}

int ProcessesItemModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(ProcessColumns::Count);
}

QVariant ProcessesItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case static_cast<int>(ProcessColumns::Pid):
            return QVariant(tr("PID"));
        case static_cast<int>(ProcessColumns::Command):
            return QVariant(tr("Command"));
    }

    return QVariant();
}

QVariant ProcessesItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    std::shared_ptr<ProcessEntry> entry = this->procData.at(index.row());

    switch (index.column()) {
        case (static_cast<int>(ProcessColumns::Pid)):
            return QVariant(entry->pid);
        case (static_cast<int>(ProcessColumns::Command)):
            return QVariant(entry->command);
    }

    return QVariant();
}

void ProcessesItemModel::clear()
{
    emit layoutAboutToBeChanged();

    this->procData.clear();

    emit layoutChanged();
}

void ProcessesItemModel::updateData(QString data)
{
    this->clear();

    emit layoutAboutToBeChanged();

    QStringList lines = data.trimmed().split("\n");

    // remove the two header lines
    if (lines.count() > 1) {
        lines.removeFirst();
    }

    QRegularExpression psRegex("^(\\d+).*?:\\d\\d (.+)$");

    std::vector<std::shared_ptr<ProcessEntry>> entries;

    for (int i = 0; i < lines.count(); i++) {
        QString line = lines.at(i).trimmed();

        QRegularExpressionMatch match = psRegex.match(line);
        if (!match.hasMatch()) {
            continue;
        }

        QString command = match.captured(2);
        // filter kernel threads
        if (command.startsWith("[")) {
            continue;
        }

        std::shared_ptr<ProcessEntry> entry = std::make_shared<ProcessEntry>();
        entries.push_back(entry);

        entry->pid = match.captured(1);
        entry->command = command;
    }

    this->procData = entries;

    emit layoutChanged();
}
