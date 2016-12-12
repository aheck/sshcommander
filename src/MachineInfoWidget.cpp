#include "MachineInfoWidget.h"

MachineInfoWidget::MachineInfoWidget()
{
    this->enabled = false;
    this->widgetStack = new QStackedWidget();
    this->page = new QWidget();

    this->disabledPage = new QWidget();
    QVBoxLayout *disabledLayout = new QVBoxLayout();
    disabledLayout->setAlignment(Qt::AlignCenter);
    QLabel *disabledLabel = new QLabel("No Machine Data");
    QFont font = disabledLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    disabledLabel->setFont(font);
    disabledLabel->setStyleSheet("QLabel { color : grey; }");
    disabledLayout->addWidget(disabledLabel);
    this->disabledPage->setLayout(disabledLayout);

    this->labelHostname = new QLabel(tr("Hostname:"));
    this->labelUsername = new QLabel(tr("Username:"));
    this->labelSSHCommand = new QLabel(tr("SSH Command:"));
    this->labelSCPCommand = new QLabel(tr("Copy File Command:"));
    this->labelSCPDirCommand = new QLabel(tr("Copy Dir Command:"));

    this->valueHostname = new QLabel();
    this->valueHostname->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueUsername = new QLabel();
    this->valueUsername->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSSHCommand = new QLabel();
    this->valueSSHCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSCPCommand = new QLabel();
    this->valueSCPCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSCPDirCommand = new QLabel();
    this->valueSCPDirCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);

    this->gridLayout = new QGridLayout();

    this->gridLayout->addWidget(this->labelHostname, 0, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHostname, 0, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelUsername, 1, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueUsername, 1, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSSHCommand, 2, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSSHCommand, 2, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelSCPCommand, 3, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSCPCommand, 3, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSCPDirCommand, 4, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSCPDirCommand, 4, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(5, 1);
    this->gridLayout->setColumnStretch(3, 1);

    this->page->setLayout(this->gridLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(this->page);

    this->widgetStack->addWidget(this->disabledPage);
    this->widgetStack->addWidget(scrollArea);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(this->widgetStack);
    this->setLayout(layout);
}

void MachineInfoWidget::setMachineEnabled(bool enabled)
{
    this->enabled = enabled;

    if (this->enabled) {
        this->widgetStack->setCurrentIndex(1);
    } else {
        this->widgetStack->setCurrentIndex(0);
    }
}

void MachineInfoWidget::updateData(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->valueHostname->setText(connEntry->hostname);
    this->valueUsername->setText(connEntry->username);

    this->valueSSHCommand->setText(connEntry->generateSSHCommand());
    this->valueSCPCommand->setText(connEntry->generateSCPCommand("File2Copy", "/tmp"));
    this->valueSCPDirCommand->setText(connEntry->generateSCPCommand("Dir2Copy", "/tmp", true));
}
