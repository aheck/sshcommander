#include "ConnectionListWidget.h"

ConnectionListWidget::ConnectionListWidget(SSHConnectionItemModel *model)
{
    this->model = model;

    this->listView = new ConnectionListView();
    this->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->listView->setModel(this->model);
    this->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SIGNAL(changeConnection(QItemSelection, QItemSelection)));
    connect(this->listView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    this->toolBar = new QToolBar();
    toolBar->addAction(QIcon(":/images/applications-internet.svg"), "New Connection",
            this, SIGNAL(showNewDialog()));
    toolBar->addAction(QIcon(":/images/preferences-system.svg"), "Edit Connection",
            this, SIGNAL(editConnection()));
    toolBar->addAction(QIcon(":/images/process-stop.svg"), "Delete Connection",
            this, SIGNAL(removeConnection()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(this->toolBar);
    layout->addWidget(this->listView);

    this->setLayout(layout);
}

void ConnectionListWidget::showContextMenu(QPoint pos)
{
    QPoint globalPos = this->listView->mapToGlobal(pos);

    if (this->listView->indexAt(pos).isValid()) {
        QMenu menu;
        QAction *editAction = menu.addAction(tr("Edit"), this, SIGNAL(editConnection()));
        editAction->setIcon(QIcon(":/images/preferences-system.svg"));
        menu.addSeparator();
        QAction *deleteAction = menu.addAction(tr("Delete"), this, SIGNAL(removeConnection()));
        deleteAction->setIcon(QIcon(":/images/process-stop.svg"));

        menu.exec(globalPos);
    }
}

void ConnectionListWidget::selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    QModelIndex index = this->model->getIndexForSSHConnectionEntry(connEntry);
    this->listView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

QModelIndexList ConnectionListWidget::getSelection()
{
    return this->listView->selectionModel()->selectedIndexes();
}

void ConnectionListWidget::selectLast()
{
    QModelIndex index = this->model->index(this->model->rowCount(QModelIndex()) - 1, 0, QModelIndex());
    this->listView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}
