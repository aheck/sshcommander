#include "MachineInfoApplet.h"

MachineInfoApplet::MachineInfoApplet()
{
    this->firstShow = true;

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

    sshLayout->addRow(tr("SSH Command:"), this->valueSSHCommand);
    sshLayout->addRow(tr("Copy File Command:"), this->valueSCPCommand);
    sshLayout->addRow(tr("Copy Dir Command:"), this->valueSCPDirCommand);

    sshGroup->setLayout(sshLayout);
    groupsRightLayout->addWidget(sshGroup);

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
    if (!this->firstShow) {
        return;
    }

    this->firstShow = false;
    this->updateData();
}

void MachineInfoApplet::updateData()
{
    this->valueHostname->setText(connEntry->hostname);
    this->valueUsername->setText(connEntry->username);

    this->valueSSHCommand->setText(connEntry->generateSSHCommand());
    this->valueSCPCommand->setText(connEntry->generateSCPCommand("File2Copy", "/tmp"));
    this->valueSCPDirCommand->setText(connEntry->generateSCPCommand("Dir2Copy", "/tmp", true));

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "uname -s -r -i", this, "sshResultReceived");
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
