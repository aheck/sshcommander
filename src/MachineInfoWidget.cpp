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

    this->page->setLayout(this->gridLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(this->page);

    this->widgetStack->addWidget(this->disabledPage);
    this->widgetStack->addWidget(this->page);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(this->widgetStack);
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

void MachineInfoWidget::setMachineEnabled(bool enabled)
{
    this->enabled = enabled;

    if (this->enabled) {
        this->widgetStack->setCurrentIndex(1);
    } else {
        this->widgetStack->setCurrentIndex(0);
    }
}
