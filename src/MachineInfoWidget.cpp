#include "MachineInfoWidget.h"

MachineInfoWidget::MachineInfoWidget()
{
    QWidget *page = new QWidget();

    this->labelHostname = new QLabel("Hostname:");
    this->labelUsername = new QLabel("Username:");

    this->valueHostname = new QLabel();
    this->valueHostname->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueUsername = new QLabel();
    this->valueUsername->setTextInteractionFlags(Qt::TextSelectableByMouse);

    this->gridLayout = new QGridLayout();
    this->gridLayout->addWidget(this->labelHostname, 0, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHostname, 0, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelUsername, 1, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueUsername, 1, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(2, 1);
    this->gridLayout->setColumnStretch(2, 1);

    page->setLayout(this->gridLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(page);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(scrollArea);
    this->setLayout(layout);
}

MachineInfoWidget::~MachineInfoWidget()
{

}

void MachineInfoWidget::setHostname(const QString hostname)
{
    this->valueHostname->setText(hostname);
}

void MachineInfoWidget::setUsername(const QString username)
{
    this->valueUsername->setText(username);
}
