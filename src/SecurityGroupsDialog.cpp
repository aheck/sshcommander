#include "SecurityGroupsDialog.h"

SecurityGroupsDialog::SecurityGroupsDialog()
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->list = new QListWidget();
    layout->addWidget(this->list);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(closeButton);

    this->setLayout(layout);
}
