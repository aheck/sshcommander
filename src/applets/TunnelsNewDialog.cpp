#include "TunnelsNewDialog.h"

TunnelsNewDialog::TunnelsNewDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("New SSH Tunnel..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));

    this->localPortLineEdit = new QSpinBox();
    this->localPortLineEdit->setMinimum(1024);
    this->localPortLineEdit->setMaximum(65535);
    this->remotePortLineEdit = new QSpinBox();
    this->remotePortLineEdit->setMinimum(1);
    this->remotePortLineEdit->setMaximum(65535);
    this->shortDescriptionLineEdit = new QLineEdit();

    SvgWidget *svgWidgetLeft = new SvgWidget(":/images/computer.svg");
    svgWidgetLeft->setFixedSize(128, 128);
    svgWidgetLeft->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    SvgWidget *svgWidgetRight = new SvgWidget(":/images/network-server.svg");
    svgWidgetRight->setFixedSize(128, 128);
    svgWidgetRight->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    SvgWidget *svgWidgetArrow = new SvgWidget(":/images/tunnel-arrow.svg");
    svgWidgetArrow->setFixedSize(188, 36);
    svgWidgetArrow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QVBoxLayout *localLayout = new QVBoxLayout();
    localLayout->addWidget(new QLabel(tr("Local TCP Port:")));
    localLayout->addWidget(this->localPortLineEdit);

    QVBoxLayout *remoteLayout = new QVBoxLayout();
    remoteLayout->addWidget(new QLabel(tr("Remote TCP Port:")));
    remoteLayout->addWidget(this->remotePortLineEdit);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(new QLabel("Local Machine"), 0, 0);
    gridLayout->addWidget(new QLabel("Remote Machine"), 0, 2);

    gridLayout->addWidget(svgWidgetLeft, 1, 0);
    gridLayout->addWidget(svgWidgetRight, 1, 2);
    gridLayout->addLayout(localLayout, 2, 0);
    gridLayout->addWidget(svgWidgetArrow, 2, 1);
    gridLayout->addLayout(remoteLayout, 2, 2);

    QVBoxLayout *descLayout = new QVBoxLayout();
    descLayout->addWidget(new QLabel(tr("Short Description:")));
    descLayout->addWidget(this->shortDescriptionLineEdit);

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
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(descLayout);
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
        msgBox.setText(tr("Local Port must not be empty!"));
        msgBox.exec();

        return;
    }

    this->remotePort = this->remotePortLineEdit->text().toInt(&ok, 10);

    if (!ok) {
        msgBox.setText(tr("Remote Port must not be empty!"));
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
