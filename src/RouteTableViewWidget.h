#ifndef ROUTETABLEVIEWWIDGET_H
#define ROUTETABLEVIEWWIDGET_H

#include <memory>

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QTableWidget>
#include <QVBoxLayout>

#include "AWSRouteTable.h"

class RouteTableViewWidget : public QWidget
{
    Q_OBJECT

public:
    RouteTableViewWidget(QWidget *parent = 0);

    void updateData(std::shared_ptr<AWSRouteTable> routeTable);
    void clear();

private slots:
    void showContextMenu(QPoint pos);
    void copyItemToClipboard();

private:
    QTableWidget *table;
    QString clipboardCandidate;
};

#endif
