#include "RouteTableViewWidget.h"

#include <QDebug>

RouteTableViewWidget::RouteTableViewWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->table = new QTableWidget(0, 2, this);
    QStringList columnNames = {"Destination", "Target"};
    this->table->setColumnCount(2);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->verticalHeader()->setVisible(false);
    this->table->setHorizontalHeaderLabels(columnNames);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table, &QTableWidget::customContextMenuRequested, this, &RouteTableViewWidget::showContextMenu);

    layout->addWidget(this->table);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
}
 
void RouteTableViewWidget::updateData(std::shared_ptr<AWSRouteTable> routeTable)
{
    this->clear();

    if (routeTable->routes.count() == 0) {
        qDebug() << "No Routes!!!";
        this->table->setRowCount(1);
        this->table->setItem(0, 0, new QTableWidgetItem("No Routes"));
        this->table->setItem(0, 1, new QTableWidgetItem(""));
        this->table->setEnabled(false);
        return;
    }

    this->table->setEnabled(true);
    this->table->setRowCount(routeTable->routes.count());

    for (int i = 0; i < routeTable->routes.count(); i++) {
        AWSRoute route = routeTable->routes.at(i);
        this->table->setItem(i, 0, new QTableWidgetItem(route.destinationCidrBlock));
        this->table->setItem(i, 1, new QTableWidgetItem(route.gatewayId));
    }
}

void RouteTableViewWidget::clear()
{
    this->table->clearContents();
    this->table->setRowCount(0);
}

void RouteTableViewWidget::showContextMenu(QPoint pos)
{
    QPoint globalPos = this->table->mapToGlobal(pos);
    QTableWidgetItem *item = this->table->itemAt(pos);

    if (item != nullptr) {
        QMenu menu;

        this->clipboardCandidate = item->data(Qt::DisplayRole).toString();
        if (this->clipboardCandidate.isEmpty()) {
            return;
        }

        menu.addAction("Copy '" + this->clipboardCandidate + "' to Clipboard", this, SLOT(copyItemToClipboard()));

        menu.exec(globalPos);
    }
}

void RouteTableViewWidget::copyItemToClipboard()
{
    QGuiApplication::clipboard()->setText(this->clipboardCandidate);
}
