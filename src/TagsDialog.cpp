#include "TagsDialog.h"

TagsDialog::TagsDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(500);
    this->setMinimumHeight(300);
    QVBoxLayout *layout = new QVBoxLayout();

    this->tagsViewer = new TagsViewWidget();
    layout->addWidget(this->tagsViewer);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, &QPushButton::clicked, this, &TagsDialog::reject);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

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

    this->tagsViewer->updateData(instance->tags);

    this->exec();
}
