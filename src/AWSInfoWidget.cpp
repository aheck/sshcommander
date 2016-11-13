#include "AWSInfoWidget.h"

AWSInfoWidget::AWSInfoWidget()
{
    this->labelInstanceId.setText("Instance ID:");
    this->labelRegion.setText("Region:");

    this->gridLayout.addWidget(&this->labelInstanceId, 0, 0, Qt::AlignLeft);
    this->gridLayout.addWidget(&this->valueInstanceId, 0, 1, Qt::AlignLeft);

    this->gridLayout.addWidget(&this->labelRegion, 1, 0, Qt::AlignLeft);
    this->gridLayout.addWidget(&this->valueRegion, 1, 1, Qt::AlignLeft);

    this->gridLayout.setRowStretch(2, 1);
    this->gridLayout.setColumnStretch(2, 1);

    this->setLayout(&this->gridLayout);
}

AWSInfoWidget::~AWSInfoWidget()
{

}

void AWSInfoWidget::setInstanceId(const QString instanceId)
{
    this->valueInstanceId.setText(instanceId);
}

void AWSInfoWidget::setRegion(const QString region)
{
    this->valueRegion.setText(region);
}
