#include "DetachedSessionWidget.h"

DetachedSessionWidget::DetachedSessionWidget()
{
    this->detachedWindow = nullptr;

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    QLabel *label = new QLabel(tr("This Session runs in a detached Window"));
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);
    label->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(label);

    QLabel *hintLabel = new QLabel(tr("(close the window to reattach it to this tab)"));
    font = hintLabel->font();
    font.setPointSize(9);
    hintLabel->setFont(font);
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(hintLabel);

    QPushButton *showButton = new QPushButton(tr("&Show Window"));
    QObject::connect(showButton, SIGNAL(clicked()), this, SLOT(showDetachedWindow()));
    layout->addWidget(showButton);

    this->setLayout(layout);
}

void DetachedSessionWidget::setUuid(QUuid uuid)
{
    this->uuid = uuid;
}

void DetachedSessionWidget::showDetachedWindow()
{
    emit requestShowWindow(this->uuid);
}
