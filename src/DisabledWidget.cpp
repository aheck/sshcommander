#include "DisabledWidget.h"

DisabledWidget::DisabledWidget(QString text)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    this->disabledLabel = new QLabel(text);
    QFont font = disabledLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    disabledLabel->setFont(font);
    disabledLabel->setStyleSheet("QLabel { color : grey; }");
    layout->addWidget(this->disabledLabel);
    setLayout(layout);

    this->setLayout(layout);
}

void DisabledWidget::setText(QString text)
{
    this->disabledLabel->setText(text);
}
