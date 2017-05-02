/*****************************************************************************
 *
 * ProcessesItemModel is the model behind the table showing running processes
 * on remote machines.
 *
 ****************************************************************************/

#ifndef PROCESSESITEMMODEL_H
#define PROCESSESITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

enum class ProcessColumns {Pid = 0, Command, Count};

struct ProcessEntry {
    QString pid;
    QString command;
};

class ProcessesItemModel : public QAbstractItemModel
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

    void updateData(QString data);

private:
    std::vector<std::shared_ptr<ProcessEntry>> procData;
};

#endif
