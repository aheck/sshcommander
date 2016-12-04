#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <memory>

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include "AWSInstance.h"

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    TagsDialog();

    void showDialog(std::shared_ptr<AWSInstance> instance);

private:
    QListWidget *list;
};

#endif
