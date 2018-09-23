#include "PortsNewTunnelDialog.h"

PortsNewTunnelDialog::PortsNewTunnelDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("New SSH Tunnel..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));

    this->localPortLineEdit = new QLineEdit();
    this->shortDescriptionLineEdit = new QLineEdit();

    this->formLayout = new QFormLayout;
#ifdef Q_OS_MACOS
    this->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
#endif
    this->formLayout->addRow(tr("Local Port:"), this->localPortLineEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);

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
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);

    bool ok;
    this->localPort = this->localPortLineEdit->text().toInt(&ok, 10);

    if (!ok) {
        msgBox.setText(tr("Local Port must be a number!"));
        msgBox.exec();

        return;
    } else if (this->localPort < 1 || this->localPort > 65535) {
        msgBox.setText(tr("Local Port must be between 1 and 65535"));
        msgBox.exec();

        return;
    }

    this->accept();
}

const int PortsNewTunnelDialog::getLocalPort()
{
    return this->localPort;
}

const QString PortsNewTunnelDialog::getShortDescription()
{
    return this->shortDescriptionLineEdit->text();
}

void PortsNewTunnelDialog::clear()
{
    this->localPortLineEdit->clear();
    this->shortDescriptionLineEdit->clear();
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
    this->descriptionLabel->setText("Create a tunnel from a port on your local machine '" +
            QHostInfo::localHostName() + "' to port " + QString::number(this->remotePort) +
            " on the remote host '" + this->remoteHostname + "':");
}

int PortsNewTunnelDialog::getRemotePort()
{
    return this->remotePort;
}
