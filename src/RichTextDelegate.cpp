#include "RichTextDelegate.h"

QWidget* RichTextDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLabel *label = new QLabel(parent);

    return label;
}

void RichTextDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QLabel *label = static_cast<QLabel*>(editor);
    label->setText(value);
    label->setOpenExternalLinks(true);
}

void RichTextDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
}

void RichTextDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
