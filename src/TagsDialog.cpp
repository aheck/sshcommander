#include "TagsDialog.h"

TagsDialog::TagsDialog()
{
    this->setMinimumWidth(300);
    QVBoxLayout *layout = new QVBoxLayout();

    this->list = new QListWidget();
    layout->addWidget(this->list);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(closeButton);

    this->setLayout(layout);
}

void TagsDialog::showDialog(std::shared_ptr<AWSInstance> instance)
{
    QString title;
    if (instance->name.isEmpty()) {
        title = QString("Tags of instance %1").arg(instance->id);
    } else {
        title = QString("Tags of instance '%1' (%2)").arg(instance->name).arg(instance->id);
    }

    this->setWindowTitle(title);

    this->list->clear();

    if (instance->tags.count() > 0) {
        for (AWSTag tag : instance->tags) {
            this->list->addItem(tag.key + " = " + tag.value);
        }
    } else {
        this->list->addItem("No Tags");
    }

    this->exec();
}
