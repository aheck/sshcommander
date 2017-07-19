#include "InactiveSessionWidget.h"

InactiveSessionWidget::InactiveSessionWidget(QUuid uuid)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    QLabel *label = new QLabel("This Session is Inactive");
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);
    label->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(label);

    QPushButton *connectButton = new QPushButton(tr("&Connect"));
    QObject::connect(connectButton, SIGNAL(clicked()), this, SLOT(createSessionSlot()));
    layout->addWidget(connectButton);

    this->uuid = uuid;
    this->setLayout(layout);
}

void InactiveSessionWidget::createSessionSlot()
{
    emit createSession(this->uuid);
}
