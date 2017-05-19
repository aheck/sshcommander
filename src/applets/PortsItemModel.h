/*****************************************************************************
 *
 * PortsItemModel is the model behind the table showing open ports on remote
 * machines.
 *
 ****************************************************************************/

#ifndef PORTSITEMMODEL_H
#define PORTSITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QColor>
#include <QIcon>
#include <QModelIndex>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

enum class PortColumns {Protocol = 0, LocalAddress, LocalPort, Details, ForeignAddress, ForeignPort, State, Count};

struct NetstatEntry {
    QString protocol;
    QString localAddress;
    QString localPort;
    QString foreignAddress;
    QString foreignPort;
    QString state;
};

struct KnownPort {
  bool tcp;
  bool udp;
  uint16_t portNumber;
  QString name;
  QString description;
  QString url;
};

class PortsItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
    void clear();

    void updateData(QString data);

private:
    std::vector<std::shared_ptr<NetstatEntry>> portsData;

    static std::map<QString, KnownPort> knownPorts;
};

#endif
