#include "NewDialog.h"

NewDialog::NewDialog(bool editDialog)
{
    this->setWindowTitle(tr("New SSH Connection..."));
    this->isAwsInstance = false;

    hostnameLineEdit = new QLineEdit();
    usernameLineEdit = new QLineEdit();
    shortDescriptionLineEdit = new QLineEdit();
    sshkeyLineEdit = new QLineEdit();
    portLineEdit = new QLineEdit(DEFAULT_SSH_PORT);
    portLineEdit->setEnabled(false);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->addWidget(sshkeyLineEdit);
    QPushButton *fileButton = new QPushButton("...");
    fileButton->setDefault(false);
    QObject::connect(fileButton, SIGNAL (clicked()), this, SLOT (selectKeyFile()));
    fileLayout->addWidget(fileButton);

    this->portCheckBox = new QCheckBox();
    QObject::connect(portCheckBox, SIGNAL (clicked(bool)), this, SLOT (portCheckBoxStateChanged(bool)));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Hostname:"), hostnameLineEdit);
    formLayout->addRow(tr("Username:"), usernameLineEdit);
    formLayout->addRow(tr("Short Description:"), shortDescriptionLineEdit);
    formLayout->addRow(tr("SSH Key:"), fileLayout);
    formLayout->addRow(tr("Custom SSH Port:"), portCheckBox);
    formLayout->addRow(tr("SSH Port:"), portLineEdit);

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
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    if (editDialog) {
        this->hostnameLineEdit->setEnabled(false);
        this->usernameLineEdit->setEnabled(false);
        connectButton->setText("OK");
    }
}

void NewDialog::selectKeyFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open SSH Key File"),
            QString("%1/.ssh").arg(QDir::homePath()),
            tr("All Files (*)"));

    if (!filename.isEmpty()) {
        sshkeyLineEdit->setText(filename);
    }
}

void NewDialog::acceptDialog()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);

    if (this->hostnameLineEdit->text().isEmpty()) {
        msgBox.setText(tr("Hostname must not be empty!"));
        msgBox.exec();

        return;
    } else if (this->usernameLineEdit->text().isEmpty()) {
        msgBox.setText(tr("Username must not be empty!"));
        msgBox.exec();

        return;
    }

    bool ok;
    int portNumber = this->portLineEdit->text().toInt(&ok, 10);

    if (!ok) {
        msgBox.setText(tr("Port must be a number!"));
        msgBox.exec();

        return;
    } else if (portNumber < 1 || portNumber > 65535) {
        msgBox.setText(tr("Port number must be between 1 and 65535"));
        msgBox.exec();

        return;
    }

    this->accept();
}

void NewDialog::portCheckBoxStateChanged(bool checked)
{
    if (!checked) {
        this->portLineEdit->setText(DEFAULT_SSH_PORT);
    }

    this->portLineEdit->setEnabled(checked);
}

const QString NewDialog::getHostname()
{
    return this->hostnameLineEdit->text();
}

void NewDialog::setHostname(const QString hostname)
{
    this->hostnameLineEdit->setText(hostname);
}

const QString NewDialog::getUsername()
{
    return this->usernameLineEdit->text();
}

void NewDialog::setUsername(const QString username)
{
    this->usernameLineEdit->setText(username);
}

const QString NewDialog::getShortDescription()
{
    return this->shortDescriptionLineEdit->text();
}

void NewDialog::setShortDescription(const QString shortDescription)
{
    this->shortDescriptionLineEdit->setText(shortDescription);
}

const QString NewDialog::getSSHKey()
{
    return this->sshkeyLineEdit->text();
}

void NewDialog::setSSHKey(const QString sshkey)
{
    this->sshkeyLineEdit->setText(sshkey);
}

int NewDialog::getPortNumber()
{
    return this->portLineEdit->text().toInt(nullptr, 10);
}

void NewDialog::setPortNumber(int port)
{
    this->portCheckBox->setEnabled(port == DEFAULT_SSH_PORT_NUMBER);
    this->portLineEdit->setText(QString::number(port));
}

void NewDialog::setFocusOnUsername()
{
    this->usernameLineEdit->setFocus();
}
