#include "TagsViewWidget.h"

#include <iostream>

TagsViewWidget::TagsViewWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->table = new QTableWidget(0, 2, this);
    QStringList columnNames = {"Key", "Value"};
    this->table->setColumnCount(2);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->verticalHeader()->setVisible(false);
    this->table->setHorizontalHeaderLabels(columnNames);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    layout->addWidget(this->table);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
}
 
void TagsViewWidget::updateData(QList<AWSTag> &tags)
{
    this->clear();

    if (tags.count() == 0) {
        std::cout << "No Tags!!!\n";
        this->table->setRowCount(1);
        this->table->setItem(0, 0, new QTableWidgetItem("No Tags"));
        this->table->setItem(0, 1, new QTableWidgetItem(""));
        this->table->setEnabled(false);
        return;
    }

    this->table->setEnabled(true);
    this->table->setRowCount(tags.count());

    for (int i = 0; i < tags.count(); i++) {
        AWSTag tag = tags.at(i);
        this->table->setItem(i, 0, new QTableWidgetItem(tag.key));
        this->table->setItem(i, 1, new QTableWidgetItem(tag.value));
    }
}

void TagsViewWidget::clear()
{
    this->table->clearContents();
}

void TagsViewWidget::showContextMenu(QPoint pos)
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

void TagsViewWidget::copyItemToClipboard()
{
    QGuiApplication::clipboard()->setText(this->clipboardCandidate);
}
