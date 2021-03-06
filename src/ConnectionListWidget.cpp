#include "ConnectionListWidget.h"

ConnectionListWidget::ConnectionListWidget(SSHConnectionItemModel *model)
{
    this->model = model;

    this->listView = new ConnectionListView();
    this->listView->setIconSize(QSize(24, 24));
    this->listView->setModel(this->model);
    connect(this->listView, &ConnectionListView::connectionMoved, this, &ConnectionListWidget::moveConnection);

    connect(this->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ConnectionListWidget::selectionChanged);
    connect(this->listView, &ConnectionListView::customContextMenuRequested, this, &ConnectionListWidget::showContextMenu);

    this->toolBar = new QToolBar();

#ifdef Q_OS_MACOS
    this->toolBar->setIconSize(QSize(MAC_ICON_SIZE, MAC_ICON_SIZE));
#endif

    toolBar->addAction(QIcon(":/images/applications-internet.svg"), "New Connection",
            this, &ConnectionListWidget::newDialogRequested);
    this->editAction = toolBar->addAction(QIcon(":/images/preferences-system.svg"), "Edit Connection",
            this, SLOT(editConnection()));
    this->editAction->setEnabled(false);
    this->deleteAction = toolBar->addAction(QIcon(":/images/process-stop.svg"), "Delete Connection",
            this, SLOT(removeSelectedConnection()));
    this->deleteAction->setEnabled(false);
    toolBar->addSeparator();
    this->awsConsoleAction = toolBar->addAction(QIcon(":/images/connection-type-aws.svg"), tr("Show AWS Console"));
    this->awsConsoleAction->setCheckable(true);
    connect(this->awsConsoleAction, &QAction::toggled, this, &ConnectionListWidget::awsConsoleToggled);

    this->editDialog = new NewDialog(this, true);

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

void ConnectionListWidget::moveConnection(int originRow, int targetRow)
{
    emit connectionMoved(originRow, targetRow);
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

void ConnectionListWidget::awsConsoleToggled(bool checked)
{
    if (!checked) {
        this->awsConsoleAction->setToolTip(tr("Show AWS Console"));
    } else {
        this->awsConsoleAction->setToolTip(tr("Hide AWS Console"));
    }

    emit toggleAwsConsole(checked);
}

void ConnectionListWidget::updateAWSInstances()
{
    this->model->updateAWSInstances();
}

void ConnectionListWidget::setAWSConsoleButton()
{
    this->awsConsoleAction->setChecked(true);
}

void ConnectionListWidget::resetAWSConsoleButton()
{
    this->awsConsoleAction->setChecked(false);
}

void ConnectionListWidget::disableAWSConsoleButton()
{
    this->awsConsoleAction->setEnabled(false);
}

void ConnectionListWidget::enableAWSConsoleButton()
{
    this->awsConsoleAction->setEnabled(true);
}

int ConnectionListWidget::connectionCount() const
{
    return this->listView->model()->rowCount();
}
