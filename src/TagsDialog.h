#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <memory>

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include "AWSInstance.h"
#include "TagsViewWidget.h"

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    TagsDialog();

    void showDialog(std::shared_ptr<AWSInstance> instance);

private:
    TagsViewWidget *tagsViewer;
};

#endif
