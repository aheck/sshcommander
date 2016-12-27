#include "NewDialog.h"

NewDialog::NewDialog(bool editDialog)
{
    this->setWindowTitle(tr("New SSH Connection..."));
    this->isAwsInstance = false;

    this->hostnameLineEdit = new QLineEdit();
    this->usernameLineEdit = new QLineEdit();
    this->shortDescriptionLineEdit = new QLineEdit();
    this->passwordLineEdit = new QLineEdit();
    this->passwordLineEdit->setEchoMode(QLineEdit::Password);
    this->sshkeyComboBox = new QComboBox();
    this->sshkeyComboBox->setEditable(true);
    this->portCheckBox = new QCheckBox();
    this->portLineEdit = new QLineEdit(DEFAULT_SSH_PORT);
    this->hopCheckBox = new QCheckBox();
    this->hopComboBox = new QComboBox();
    this->hopComboBox->setEditable(true);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->addWidget(this->sshkeyComboBox);
    QToolButton *fileButton = new QToolButton();
    fileButton->setText("...");
    QObject::connect(fileButton, SIGNAL (clicked()), this, SLOT (selectKeyFile()));
    fileLayout->addWidget(fileButton);

    QObject::connect(this->portCheckBox, SIGNAL (clicked(bool)), this, SLOT (portCheckBoxStateChanged(bool)));
    QObject::connect(this->hopCheckBox, SIGNAL (clicked(bool)), this, SLOT (hopCheckBoxStateChanged(bool)));

    this->formLayout = new QFormLayout;
    this->formLayout->addRow(tr("Hostname:"), this->hostnameLineEdit);
    this->formLayout->addRow(tr("Username:"), this->usernameLineEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);
    this->formLayout->addRow(tr("Password:"), this->passwordLineEdit);
    this->formLayout->addRow(tr("SSH Key:"), fileLayout);
    this->formLayout->addRow(tr("Custom SSH Port:"), this->portCheckBox);
    this->formLayout->addRow(tr("SSH Port:"), this->portLineEdit);
    this->formLayout->addRow(tr("Hop over other SSH host:"), this->hopCheckBox);
    this->formLayout->addRow(tr("Hop host:"), this->hopComboBox);

    portCheckBoxStateChanged(this->portCheckBox->isChecked());
    hopCheckBoxStateChanged(this->hopCheckBox->isChecked());

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

    if (editDialog) {
        this->hostnameLineEdit->setEnabled(false);
        this->usernameLineEdit->setEnabled(false);
        connectButton->setText("OK");
    }

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void NewDialog::selectKeyFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open SSH Key File"),
            QString("%1/.ssh").arg(QDir::homePath()),
            tr("All Files (*)"));

    if (!filename.isEmpty()) {
        sshkeyComboBox->setCurrentText(filename);
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

    this->formLayout->labelForField(this->portLineEdit)->setVisible(checked);
    this->portLineEdit->setVisible(checked);
}

void NewDialog::hopCheckBoxStateChanged(bool checked)
{
    this->formLayout->labelForField(this->hopComboBox)->setVisible(checked);
    this->hopComboBox->setVisible(checked);
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

const QString NewDialog::getPassword()
{
    return this->passwordLineEdit->text();
}

void NewDialog::setPassword(const QString &password)
{
    this->passwordLineEdit->setText(password);
}

const QString NewDialog::getSSHKey()
{
    return this->sshkeyComboBox->currentText();
}

void NewDialog::setSSHKey(const QString sshkey)
{
    int index = this->sshkeyComboBox->findText(sshkey);
    this->sshkeyComboBox->setCurrentIndex(index);
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

void NewDialog::updateSSHKeys()
{
    QStringList sshkeys;

    sshkeys.append("");

    QDirIterator it(QDir::homePath() + "/.ssh");
    while (it.hasNext()) {
        QString dir = it.next();
        if (dir.endsWith("/.") || dir.endsWith("/..") ||
                dir.endsWith(".pub") || dir.endsWith("/config") ||
                dir.endsWith("/known_hosts")) {
            continue;
        }

        sshkeys.append(dir);
    }

    this->sshkeyComboBox->clear();
    this->sshkeyComboBox->addItems(sshkeys);
}

QStringList NewDialog::getHopHosts()
{
    QStringList hosts;

    if (this->hopCheckBox->isChecked()) {
        int index = this->hopComboBox->currentIndex();

        if (this->hopComboBox->itemData(index).toString().isEmpty()) {
            hosts.append(this->hopComboBox->currentText());
        } else {
            hosts.append(this->hopComboBox->itemData(index).toString());
        }
    }

    return hosts;
}

void NewDialog::setHopChecked(bool checked)
{
    this->hopCheckBox->setChecked(checked);
    this->hopCheckBoxStateChanged(checked);
}

void NewDialog::addHopHost(QString name, QString ip)
{
    this->hopComboBox->addItem(name, QVariant(ip));
}

void NewDialog::clear()
{
    this->hostnameLineEdit->clear();
    this->usernameLineEdit->clear();
    this->shortDescriptionLineEdit->clear();
    this->passwordLineEdit->clear();
    this->sshkeyComboBox->clear();
    this->sshkeyComboBox->clear();
    this->portCheckBox->setChecked(false);
    this->portCheckBoxStateChanged(false);
    this->portLineEdit->setText(DEFAULT_SSH_PORT);
    this->hopCheckBox->setChecked(false);
    this->hopCheckBoxStateChanged(false);
    this->hopComboBox->clear();
}

const QString NewDialog::findSSHKey(const QString keyname)
{
    QString result;

    QString keynameInDotSSH = QDir::homePath() + "/.ssh/" + keyname;
    if (QFileInfo::exists(keynameInDotSSH)) {
        QFileInfo info(keynameInDotSSH);

        if (info.isFile()) {
            result = keynameInDotSSH;
        }
    }

    return result;
}
