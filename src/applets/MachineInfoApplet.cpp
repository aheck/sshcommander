#include "MachineInfoApplet.h"

MachineInfoApplet::MachineInfoApplet()
{
    this->firstShow = true;
    connect(&this->fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateKnownHostsData()));

    this->page = new QWidget();
    this->page->setObjectName("scrollAreaContent");

    this->valueHostname = new QLabel();
    this->valueHostname->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueUsername = new QLabel();
    this->valueUsername->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueOperatingSystem = new QLabel();
    this->valueOperatingSystem->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueDistro = new QLabel();
    this->valueDistro->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueCpu = new QLabel();
    this->valueCpu->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueMemory = new QLabel();
    this->valueMemory->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSSHCommand = new QLabel();
    this->valueSSHCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSCPCommand = new QLabel();
    this->valueSCPCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueSCPDirCommand = new QLabel();
    this->valueSCPDirCommand->setTextInteractionFlags(Qt::TextSelectableByMouse);

    this->valueKnownHostsFile = new QLabel();
    this->valueKnownHostsFile->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueFileExists = new QLabel();
    this->valueFileExists->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->valueKnownHostsEntryExists = new QLabel();
    this->valueKnownHostsEntryExists->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QHBoxLayout *groupsLayout = new QHBoxLayout();
    QVBoxLayout *groupsLeftLayout = new QVBoxLayout();
    groupsLeftLayout->setAlignment(Qt::AlignTop);
    QVBoxLayout *groupsRightLayout = new QVBoxLayout();
    groupsRightLayout->setAlignment(Qt::AlignTop);

    groupsLayout->addLayout(groupsLeftLayout);
    groupsLayout->addLayout(groupsRightLayout);

    QFormLayout *generalLayout = new QFormLayout();
    QGroupBox *generalGroup = new QGroupBox(tr("General"));
    generalGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    generalLayout->addRow(tr("Hostname:"), this->valueHostname);
    generalLayout->addRow(tr("Username:"), this->valueUsername);

    generalGroup->setLayout(generalLayout);
    groupsLeftLayout->addWidget(generalGroup);

    QFormLayout *systemLayout = new QFormLayout();
    QGroupBox *systemGroup = new QGroupBox(tr("System"));
    systemGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    systemLayout->addRow(tr("Operating System:"), this->valueOperatingSystem);
    systemLayout->addRow(tr("Distro:"), this->valueDistro);
    systemLayout->addRow(tr("CPU(s):"), this->valueCpu);
    systemLayout->addRow(tr("RAM:"), this->valueMemory);

    systemGroup->setLayout(systemLayout);
    groupsLeftLayout->addWidget(systemGroup);

    QFormLayout *sshLayout = new QFormLayout();
    QGroupBox *sshGroup = new QGroupBox(tr("SSH"));
    sshGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    sshLayout->addRow(tr("SSH Connect:"), this->valueSSHCommand);
    sshLayout->addRow(tr("Copy File Command:"), this->valueSCPCommand);
    sshLayout->addRow(tr("Copy Dir Command:"), this->valueSCPDirCommand);

    sshGroup->setLayout(sshLayout);
    groupsRightLayout->addWidget(sshGroup);

    QFormLayout *knownHostsLayout = new QFormLayout();
    QGroupBox *knownHostsGroup = new QGroupBox(tr("Local Known Hosts"));
    knownHostsGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    knownHostsLayout->addRow(tr("Known Hosts File:"), this->valueKnownHostsFile);
    knownHostsLayout->addRow(tr("File Exists:"), this->valueFileExists);
    this->removeHostButton = new QPushButton(this->tr("Remove Host"));
    connect(this->removeHostButton, SIGNAL(clicked()), this, SLOT(removeHostFromKnownHosts()));
    this->removeHostButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    knownHostsLayout->addRow(tr("Host exists in Known Hosts File:"), this->valueKnownHostsEntryExists);
    knownHostsLayout->addRow(tr("Remove this Host from Known Hosts:"), this->removeHostButton);

    knownHostsGroup->setLayout(knownHostsLayout);
    groupsRightLayout->addWidget(knownHostsGroup);

    this->page->setLayout(groupsLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(this->page);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(scrollArea);
    this->setLayout(layout);
}

const QString MachineInfoApplet::getDisplayName()
{
    return tr("Machine");
}

QIcon MachineInfoApplet::getIcon()
{
    return QIcon(":/images/computer.svg");
}

void MachineInfoApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
}

void MachineInfoApplet::onShow()
{
    this->updateKnownHostsData();

    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void MachineInfoApplet::updateData()
{
    this->valueHostname->setText(this->connEntry->hostname);
    this->valueUsername->setText(this->connEntry->username);

    this->valueSSHCommand->setText(this->connEntry->generateSSHCommand());
    this->valueSCPCommand->setText(this->connEntry->generateSCPCommand("File2Copy", "/tmp"));
    this->valueSCPDirCommand->setText(this->connEntry->generateSCPCommand("Dir2Copy", "/tmp", true));

    this->updateKnownHostsData();

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "uname -s -r -i", this, "sshResultReceived");
}

void MachineInfoApplet::updateKnownHostsData()
{
    this->knownHostsFilePath = this->getKnownHostsFilePath();
    // With QFileSystemWatcher we can only add the path once so calling
    // it with each update makes sure it is always set, even when the
    // known hosts file gets deleted and recreated in the meantime
    this->fileWatcher.addPath(this->knownHostsFilePath);

    this->valueKnownHostsFile->setText(this->knownHostsFilePath);
    if (QFile::exists(this->knownHostsFilePath)) {
        this->valueFileExists->setText(tr("Yes"));
        this->valueFileExists->setStyleSheet("QLabel { font-weight: bold; color: green;}");
    } else {
        this->valueFileExists->setText(tr("No"));
        this->valueFileExists->setStyleSheet("QLabel { font-weight: bold; color: goldenrod;}");
    }
    if (this->isHostInKnownHostsFile()) {
        this->valueKnownHostsEntryExists->setText(tr("Yes"));
        this->valueKnownHostsEntryExists->setStyleSheet("QLabel { font-weight: bold; color: green;}");
        this->removeHostButton->setEnabled(true);
    } else {
        this->valueKnownHostsEntryExists->setText(tr("No"));
        this->valueKnownHostsEntryExists->setStyleSheet("QLabel { font-weight: bold; color: goldenrod;}");
        this->removeHostButton->setEnabled(false);
    }
}

void MachineInfoApplet::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
{
    if (!cmdResult->isSuccess) {
        std::cout << "ERROR: SSH remote command failed: " << cmdResult->errorString.toStdString() << std::endl;
        return;
    }

    if (cmdResult->command.startsWith("uname ")) {
        QString os = cmdResult->resultString.trimmed();
        this->valueOperatingSystem->setText(os);

        if (os.startsWith("Linux ")) {
            SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();

            connMgr.executeRemoteCmd(this->connEntry, "lsb_release -sd", this, "sshResultReceived");
            connMgr.executeRemoteCmd(this->connEntry, "grep 'model name' /proc/cpuinfo", this, "sshResultReceived");
            connMgr.executeRemoteCmd(this->connEntry, "grep MemTotal: /proc/meminfo", this, "sshResultReceived");
        } else if (os.startsWith("FreeBSD")) {

        }
    } else if (cmdResult->command.startsWith("lsb_release ")) {
        this->valueDistro->setText(cmdResult->resultString.trimmed());
    } else if (cmdResult->command.startsWith("grep MemTotal: ")) {
        QString mem = cmdResult->resultString.remove("MemTotal:").trimmed();

        if (mem.endsWith(" kB")) {
            mem.remove(" kB");
            QString unit = "MB";
            double memSize = mem.toDouble();
            memSize /= 1024;
            if (memSize > 1023) {
                unit = "GB";
                memSize /= 1024;
            }

            mem = QString::number(memSize, 'f', 1) + " " + unit;
        }

        this->valueMemory->setText(mem);
    } else if (cmdResult->command.startsWith("grep 'model name' ")) {
        QString cpu = cmdResult->resultString.remove("model name").remove(": ").trimmed();
        cpu = cpu.replace(QRegExp("\n\\s+"), "\n");

        QStringList lines = cpu.split("\n");

        if (lines.count() > 1) {
            cpu = "";
            QString currentLine = lines.at(0);
            int num = 0;

            for (auto line : lines) {
                num++;

                if (line != currentLine) {
                    cpu += QString::number(num) + " x " + currentLine + "\n";
                    currentLine = line;
                    num = 1;
                }
            }

            cpu += QString::number(num) + " x " + currentLine;
        } else {
            cpu = "1 x " + cpu;
        }

        this->valueCpu->setText(cpu);
    }
}

QString MachineInfoApplet::getKnownHostsFilePath()
{
    QString path;

    path = QDir::homePath() + "/.ssh/known_hosts";

    return path;
}

void MachineInfoApplet::removeHostFromKnownHosts()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Really Remove this Host?",
            QString("The known hosts file stores the identities of hosts you have already connected to. "
                "When SSH refuses to connect because the identity of a host does not match the identity "
                "stored in the known hosts file an attacker might have manipulated your connection or "
                "in some cases he even hacked your host.\n\n"
                "If you don't know why the identity of your target host changed you should "
                "investigate why this happened instead of just deleting the known hosts entry.\n\n"
                "Do you really want to remove this host from your known hosts file?"),
            QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    bool success = this->removeHostFromKnownHostsFile();

    if (!success) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Failed to remove host '" + this->connEntry->hostname +
                "' from known hosts file '" + this->knownHostsFilePath + "'");
        msgBox.exec();
    }

    this->updateKnownHostsData();
}

bool MachineInfoApplet::isHostInKnownHostLine(QString hostname, QString line)
{
    QStringList fields = line.split(" ");
    if (fields.length() < 2) {
        return false;
    }

    QString hostField = fields.at(0);

    // SHA1 HMAC hashed hostnames?
    if (hostField.startsWith("|1|")) {
        hostField.remove(0, 3); // remove hash marker |1|
        QStringList hashFields = hostField.split("|");
        if (hashFields.length() != 2) {
            return false;
        }

        QByteArray key = QByteArray::fromBase64(hashFields.at(0).toLatin1());
        QByteArray hashedHostname = QByteArray::fromBase64(hashFields.at(1).toLatin1());

        QByteArray hostnameHashed = QMessageAuthenticationCode::hash(hostname.toLatin1(), key, QCryptographicHash::Sha1);

        if (hashedHostname == hostnameHashed) {
            return true;
        }
    } else { // plain text hostnames
        QStringList hostnames = hostField.split(",");

        for (QString curHostname : hostnames) {
            if (curHostname == hostname) {
                return true;
            }
        }
    }

    return false;
}

bool MachineInfoApplet::isHostInKnownHostsFile()
{
    QFile file(this->knownHostsFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (isHostInKnownHostLine(this->connEntry->hostname, line)) {
            return true;
        }
    }

    return false;
}

bool MachineInfoApplet::removeHostFromKnownHostsFile()
{
    QStringList linesToWrite;

    QFile file(this->knownHostsFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        QString line = QString::fromLatin1(lineBytes);

        if (!isHostInKnownHostLine(this->connEntry->hostname, line)) {
            linesToWrite.append(line);
        }
    }

    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    for (QString line : linesToWrite) {
        file.write(line.toLatin1());
    }

    return true;
}
