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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignHCenter);
    QPushButton *connectButton = new QPushButton(tr("&Connect"));
    connectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QObject::connect(connectButton, &QPushButton::clicked, this, &InactiveSessionWidget::createSessionSlot);
    buttonLayout->addWidget(connectButton);

    layout->addLayout(buttonLayout);

    this->uuid = uuid;
    this->setLayout(layout);
}

void InactiveSessionWidget::createSessionSlot()
{
    emit createSession(this->uuid);
}
