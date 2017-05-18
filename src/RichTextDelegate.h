/*****************************************************************************
 *
 * RichTextDelegate is a delegate for a QTableView which renders rich text in
 * a cell (which has to be put in edit mode).
 *
 ****************************************************************************/

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextDocument>
#include <QLabel>

class RichTextDelegate : public QStyledItemDelegate
{
protected:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
