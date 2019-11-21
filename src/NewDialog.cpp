#include "NewDialog.h"

NewDialog::NewDialog(QWidget *parent, bool editDialog)
    : QDialog(parent)
{
    this->setWindowTitle(tr("New SSH Connection..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));
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
    this->hopHostBox = new QComboBox();
    this->hopHostBox->setEditable(true);
    this->hopUsernameLineEdit = new QLineEdit();
    this->hopSSHKeyComboBox = new QComboBox();
    this->hopSSHKeyComboBox->setEditable(true);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->addWidget(this->sshkeyComboBox);
    QToolButton *fileButton = new QToolButton();
    fileButton->setText("...");
    QObject::connect(fileButton, &QToolButton::clicked, this, &NewDialog::selectKeyFileDefault);
    fileLayout->addWidget(fileButton);

    this->hopFileLayout = new QHBoxLayout();
    hopFileLayout->addWidget(this->hopSSHKeyComboBox);
    this->hopFileButton = new QToolButton();
    this->hopFileButton->setText("...");
    QObject::connect(this->hopFileButton, &QToolButton::clicked, this, &NewDialog::selectKeyFileHop);
    hopFileLayout->addWidget(this->hopFileButton);

    QObject::connect(this->portCheckBox, &QCheckBox::clicked, this, &NewDialog::portCheckBoxStateChanged);
    QObject::connect(this->hopCheckBox, &QCheckBox::clicked, this, &NewDialog::hopCheckBoxStateChanged);

    this->formLayout = new QFormLayout;
#ifdef Q_OS_MACOS
    this->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
#endif
    this->formLayout->addRow(tr("Hostname:"), this->hostnameLineEdit);
    this->formLayout->addRow(tr("Username:"), this->usernameLineEdit);
    this->formLayout->addRow(tr("Password:"), this->passwordLineEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);
    this->formLayout->addRow(tr("SSH Key:"), fileLayout);
    this->formLayout->addRow(tr("Custom SSH Port:"), this->portCheckBox);
    this->formLayout->addRow(tr("SSH Port:"), this->portLineEdit);
    this->formLayout->addRow(tr("Hop over other SSH host:"), this->hopCheckBox);
    this->formLayout->addRow(tr("Hop Host:"), this->hopHostBox);
    this->formLayout->addRow(tr("Hop Username:"), this->hopUsernameLineEdit);
    this->formLayout->addRow(tr("Hop SSH Key:"), hopFileLayout);

    portCheckBoxStateChanged(this->portCheckBox->isChecked());
    hopCheckBoxStateChanged(this->hopCheckBox->isChecked());

    QPushButton *connectButton = new QPushButton(tr("Connect"));
    QObject::connect(connectButton, &QPushButton::clicked, this, &NewDialog::acceptDialog);
    connectButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, &QPushButton::clicked, this, &NewDialog::reject);
    cancelButton->setDefault(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(connectButton);

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

void NewDialog::selectKeyFileDefault()
{
    this->selectKeyFile(this->sshkeyComboBox);
}

void NewDialog::selectKeyFileHop()
{
    this->selectKeyFile(this->hopSSHKeyComboBox);
}

void NewDialog::selectKeyFile(QComboBox *targetComboBox)
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open SSH Key File"),
            QString("%1/.ssh").arg(QDir::homePath()),
            tr("All Files (*)"));

    if (!filename.isEmpty()) {
        targetComboBox->setCurrentText(filename);
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
    this->formLayout->labelForField(this->hopHostBox)->setVisible(checked);
    this->hopHostBox->setVisible(checked);
    this->formLayout->labelForField(this->hopUsernameLineEdit)->setVisible(checked);
    this->hopUsernameLineEdit->setVisible(checked);
    this->formLayout->labelForField(this->hopFileLayout)->setVisible(checked);
    this->hopSSHKeyComboBox->setVisible(checked);
    this->hopFileButton->setVisible(checked);
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
    this->hopSSHKeyComboBox->clear();
    this->hopSSHKeyComboBox->addItems(sshkeys);
}

QStringList NewDialog::getHopHosts()
{
    QStringList hosts;

    if (this->hopCheckBox->isChecked()) {
        int index = this->hopHostBox->currentIndex();

        if (this->hopHostBox->itemData(index).toString().isEmpty()) {
            hosts.append(this->hopHostBox->currentText());
        } else {
            hosts.append(this->hopHostBox->itemData(index).toString());
        }
    }

    return hosts;
}

QStringList NewDialog::getHopUsernames()
{
    QStringList usernames;

    if (this->hopCheckBox->isChecked()) {
        usernames.append(this->hopUsernameLineEdit->text());
    }

    return usernames;
}

QStringList NewDialog::getHopSSHKeys()
{
    QStringList sshkeys;

    if (this->hopCheckBox->isChecked()) {
        sshkeys.append(this->sshkeyComboBox->currentText());
    }

    return sshkeys;
}

void NewDialog::setHopChecked(bool checked)
{
    this->hopCheckBox->setChecked(checked);
    this->hopCheckBoxStateChanged(checked);
}

void NewDialog::addHopHost(QString name, QString ip)
{
    this->hopHostBox->addItem(name, QVariant(ip));
}

void NewDialog::clear()
{
    this->hostnameLineEdit->clear();
    this->usernameLineEdit->clear();
    this->shortDescriptionLineEdit->clear();
    this->passwordLineEdit->clear();
    this->sshkeyComboBox->clear();
    this->portCheckBox->setChecked(false);
    this->portCheckBoxStateChanged(false);
    this->portLineEdit->setText(DEFAULT_SSH_PORT);
    this->hopCheckBox->setChecked(false);
    this->hopCheckBoxStateChanged(false);
    this->hopHostBox->clear();
    this->hopUsernameLineEdit->clear();
    this->hopSSHKeyComboBox->clear();
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
