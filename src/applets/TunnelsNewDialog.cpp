#include "TunnelsNewDialog.h"

TunnelsNewDialog::TunnelsNewDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("New SSH Tunnel..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));

    this->localPortLineEdit = new QLineEdit();
    this->remotePortLineEdit = new QLineEdit();
    this->shortDescriptionLineEdit = new QLineEdit();

    this->formLayout = new QFormLayout;
    this->formLayout->addRow(tr("Local Port:"), this->localPortLineEdit);
    this->formLayout->addRow(tr("Remote Port:"), this->remotePortLineEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);

    QPushButton *connectButton = new QPushButton(tr("Connect"));
    QObject::connect(connectButton, SIGNAL (clicked()), this, SLOT (acceptDialog()));
    connectButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, SIGNAL (clicked()), this, SLOT(reject()));
    cancelButton->setDefault(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(connectButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(this->formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void TunnelsNewDialog::acceptDialog()
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

    this->remotePort = this->remotePortLineEdit->text().toInt(&ok, 10);

    if (!ok) {
        msgBox.setText(tr("Remote Port must be a number!"));
        msgBox.exec();

        return;
    } else if (this->remotePort < 1 || this->remotePort > 65535) {
        msgBox.setText(tr("Remote Port must be between 1 and 65535"));
        msgBox.exec();

        return;
    }

    if (TunnelManager::isLocalPortInUse(this->localPort)) {
        msgBox.setText(QString("Local port ") + QString::number(this->localPort) + " is already used by another process");
        msgBox.exec();

        return;
    }

    this->accept();
}

const int TunnelsNewDialog::getLocalPort()
{
    return this->localPort;
}

const int TunnelsNewDialog::getRemotePort()
{
    return this->remotePort;
}

const QString TunnelsNewDialog::getShortDescription()
{
    return this->shortDescriptionLineEdit->text();
}

void TunnelsNewDialog::clear()
{
    this->localPortLineEdit->clear();
    this->remotePortLineEdit->clear();
    this->shortDescriptionLineEdit->clear();
}
