#include "MachineInfoWidget.h"

MachineInfoWidget::MachineInfoWidget()
{
    this->labelHostname.setText("Hostname:");
    this->labelUsername.setText("Username:");

    this->gridLayout.addWidget(&this->labelHostname, 0, 0, Qt::AlignLeft);
    this->gridLayout.addWidget(&this->valueHostname, 0, 1, Qt::AlignLeft);

    this->gridLayout.addWidget(&this->labelUsername, 1, 0, Qt::AlignLeft);
    this->gridLayout.addWidget(&this->valueUsername, 1, 1, Qt::AlignLeft);

    this->gridLayout.setRowStretch(2, 1);
    this->gridLayout.setColumnStretch(2, 1);

    this->setLayout(&this->gridLayout);
}

MachineInfoWidget::~MachineInfoWidget()
{

}

void MachineInfoWidget::setHostname(const QString hostname)
{
    this->valueHostname.setText(hostname);
}

void MachineInfoWidget::setUsername(const QString username)
{
    this->valueUsername.setText(username);
}
