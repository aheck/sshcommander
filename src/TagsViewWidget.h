/*****************************************************************************
 *
 * TagsViewWidget is the widget used to show AWS tags.
 *
 ****************************************************************************/

#ifndef TAGSVIEWWIDGET_H
#define TAGSVIEWWIDGET_H

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QTableWidget>
#include <QVBoxLayout>

#include "AWSTag.h"

class TagsViewWidget : public QWidget
{
    Q_OBJECT

public:
    TagsViewWidget(QWidget *parent = 0);

    void updateData(QList<AWSTag> &tags);
    void clear();

private slots:
    void showContextMenu(QPoint pos);
    void copyItemToClipboard();

private:
    QTableWidget *table;
    QString clipboardCandidate;
};

#endif
