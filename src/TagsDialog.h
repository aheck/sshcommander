/*****************************************************************************
 *
 * TagsDialog is the dialog used to show AWS tags.
 *
 ****************************************************************************/

#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <memory>

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include "AWS/AWSInstance.h"
#include "TagsViewWidget.h"

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    TagsDialog(QWidget *parent = nullptr);

    void showDialog(std::shared_ptr<AWSInstance> instance);

private:
    TagsViewWidget *tagsViewer;
};

#endif
