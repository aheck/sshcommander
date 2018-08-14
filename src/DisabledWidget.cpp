#include "DisabledWidget.h"

DisabledWidget::DisabledWidget(QString text)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignCenter);

    this->disabledLabel = new QLabel(text);
    QFont font = this->disabledLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    this->disabledLabel->setFont(font);
    this->disabledLabel->setStyleSheet("QLabel { color : grey; }");

    QHBoxLayout *descriptionLayout = new QHBoxLayout();
    descriptionLayout->setAlignment(Qt::AlignHCenter);
    this->descriptionLabel = new QLabel();
    font = this->descriptionLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    this->descriptionLabel->setFont(font);
    this->descriptionLabel->setStyleSheet("QLabel { color : #555; }");
    this->descriptionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    descriptionLayout->addWidget(this->descriptionLabel);

    layout->addWidget(this->disabledLabel);
    layout->addLayout(descriptionLayout);

    this->setLayout(layout);
}

void DisabledWidget::setText(QString text)
{
    this->disabledLabel->setText(text);
}

void DisabledWidget::setDescription(QString description)
{
    this->descriptionLabel->setText(description);
}
