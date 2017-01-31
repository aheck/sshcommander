#include "ConnectionListWidget.h"

ConnectionListWidget::ConnectionListWidget(SSHConnectionItemModel *model)
{
    this->model = model;

    this->listView = new ConnectionListView();
    this->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->listView->setModel(this->model);
    this->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
    connect(this->listView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    this->toolBar = new QToolBar();
    toolBar->addAction(QIcon(":/images/applications-internet.svg"), "New Connection",
            this, SIGNAL(newDialogRequested()));
    this->editAction = toolBar->addAction(QIcon(":/images/preferences-system.svg"), "Edit Connection",
            this, SLOT(editConnection()));
    this->editAction->setEnabled(false);
    this->deleteAction = toolBar->addAction(QIcon(":/images/process-stop.svg"), "Delete Connection",
            this, SLOT(removeSelectedConnection()));
    this->deleteAction->setEnabled(false);

    this->editDialog = new NewDialog(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->toolBar);
    layout->addWidget(this->listView);

    this->setLayout(layout);
}

ConnectionListWidget::~ConnectionListWidget()
{
    delete this->editDialog;
}

void ConnectionListWidget::showContextMenu(QPoint pos)
{
    QPoint globalPos = this->listView->mapToGlobal(pos);

    if (this->listView->indexAt(pos).isValid()) {
        QMenu menu;
        QAction *editAct = menu.addAction(tr("Edit"), this, SLOT(editConnection()));
        editAct->setIcon(QIcon(":/images/preferences-system.svg"));
        menu.addSeparator();
        QAction *deleteAct = menu.addAction(tr("Delete"), this, SLOT(removeSelectedConnection()));
        deleteAct->setIcon(QIcon(":/images/process-stop.svg"));

        menu.exec(globalPos);
    }
}

void ConnectionListWidget::selectConnection(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    QModelIndex index = this->model->getIndexForSSHConnectionEntry(connEntry);
    this->listView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);

    this->editAction->setEnabled(true);
    this->deleteAction->setEnabled(true);
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

void ConnectionListWidget::editConnection()
{
    auto connEntry = this->getSelectedConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    this->editDialog->clear();
    this->editDialog->updateSSHKeys();

    this->editDialog->setWindowTitle("Edit " + connEntry->name);
    this->editDialog->setHostname(connEntry->hostname);
    this->editDialog->setUsername(connEntry->username);
    this->editDialog->setShortDescription(connEntry->shortDescription);
    this->editDialog->setPassword(connEntry->password);
    this->editDialog->setSSHKey(connEntry->sshkey);
    this->editDialog->setPortNumber(connEntry->port);
    this->editDialog->setHopChecked(connEntry->hopHosts.count() > 0);

    if (this->editDialog->exec() == QDialog::Rejected) {
        return;
    }

    connEntry->shortDescription = this->editDialog->getShortDescription();
    connEntry->sshkey = this->editDialog->getSSHKey();
    connEntry->password = this->editDialog->getPassword();
    connEntry->port = this->editDialog->getPortNumber();
}

std::shared_ptr<SSHConnectionEntry> ConnectionListWidget::getSelectedConnectionEntry()
{
    QModelIndexList indexes = this->getSelection();
    if (indexes.isEmpty()) {
        return nullptr;
    }

    std::shared_ptr<SSHConnectionEntry> connEntry = this->model->getConnEntry(indexes.first().row());

    return connEntry;
}

void ConnectionListWidget::selectionChanged(const QItemSelection &selected,
        const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();

    if (indexes.size() == 0) {
        this->editAction->setEnabled(false);
        this->deleteAction->setEnabled(false);

        return;
    }

    this->editAction->setEnabled(true);
    this->deleteAction->setEnabled(true);

    QModelIndex index = indexes.at(0);

    emit connectionChanged(index.row());
}

void ConnectionListWidget::selectFirstConnection()
{
    if (this->model->rowCount(QModelIndex()) < 1) {
        return;
    }

    std::shared_ptr<SSHConnectionEntry> entry = this->model->getConnEntry(0);
    this->selectConnection(entry);
}

const QString ConnectionListWidget::getSelectedUsernameAndHost()
{
    std::shared_ptr<SSHConnectionEntry> connEntry = this->getSelectedConnectionEntry();

    if (connEntry == nullptr) {
        return QString("");
    }

    return connEntry->name;
}

void ConnectionListWidget::removeSelectedConnection()
{
    auto connEntry = this->getSelectedConnectionEntry();

    if (connEntry == nullptr) {
        return;
    }

    QMessageBox::StandardButton reply;
    const QString usernameAndHost = this->getSelectedUsernameAndHost();
    reply = QMessageBox::question(this, "Removing Connection",
            QString("Do you really want to remove the SSH connection '%1'?").arg(usernameAndHost),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    this->model->removeConnectionEntry(connEntry);
    emit connectionRemoved(connEntry);
}
