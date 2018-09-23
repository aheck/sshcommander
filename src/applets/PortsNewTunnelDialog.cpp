#include "PortsNewTunnelDialog.h"

PortsNewTunnelDialog::PortsNewTunnelDialog(QWidget *parent)
    : QDialog(parent)
{
    this->remotePort = 0;

    this->setWindowTitle(tr("New SSH Tunnel..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));

    this->localPortEdit = new QSpinBox();
    this->localPortEdit->setRange(1024, 65535);
    this->shortDescriptionEdit = new QLineEdit();

    this->formLayout = new QFormLayout;
#ifdef Q_OS_MACOS
    this->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
#endif
    this->formLayout->addRow(tr("Local Port:"), this->localPortEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionEdit);

    QPushButton *connectButton = new QPushButton(tr("Connect"));
    QObject::connect(connectButton, SIGNAL (clicked()), this, SLOT (acceptDialog()));
    connectButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, SIGNAL (clicked()), this, SLOT(reject()));
    cancelButton->setDefault(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(connectButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->descriptionLabel = new QLabel();
    mainLayout->addWidget(this->descriptionLabel);
    mainLayout->addLayout(this->formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void PortsNewTunnelDialog::acceptDialog()
{
    this->accept();
}

const int PortsNewTunnelDialog::getLocalPort()
{
    return this->localPortEdit->value();
}

const QString PortsNewTunnelDialog::getShortDescription()
{
    return this->shortDescriptionEdit->text();
}

void PortsNewTunnelDialog::clear()
{
    this->localPortEdit->clear();
    this->shortDescriptionEdit->clear();
}

void PortsNewTunnelDialog::setRemotePort(int remotePort)
{
    this->remotePort = remotePort;
}

void PortsNewTunnelDialog::setRemoteHostname(QString remoteHostname)
{
    this->remoteHostname = remoteHostname;
}

void PortsNewTunnelDialog::update()
{
    this->descriptionLabel->setText("Create a tunnel from your local machine '" +
            QHostInfo::localHostName() + "' \nto port " + QString::number(this->remotePort) +
            " on the remote host '" + this->remoteHostname + "':");
}

int PortsNewTunnelDialog::getRemotePort()
{
    return this->remotePort;
}
