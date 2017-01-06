#include "MachineInfoWidget.h"

MachineInfoWidget::MachineInfoWidget()
{
    this->enabled = false;
    this->widgetStack = new QStackedWidget();
    this->page = new QWidget();

    this->disabledWidget = new DisabledWidget("No SSH Connection");

    this->labelHostname = new QLabel(tr("Hostname:"));
    this->labelUsername = new QLabel(tr("Username:"));
    this->labelOperatingSystem = new QLabel(tr("Operating System:"));
    this->labelDistro = new QLabel(tr("Distro:"));
    this->labelCpu = new QLabel(tr("CPU(s):"));
    this->labelMemory = new QLabel(tr("RAM:"));
    this->labelSSHCommand = new QLabel(tr("SSH Command:"));
    this->labelSCPCommand = new QLabel(tr("Copy File Command:"));
    this->labelSCPDirCommand = new QLabel(tr("Copy Dir Command:"));

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

    this->gridLayout = new QGridLayout();

    this->gridLayout->addWidget(this->labelHostname, 0, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueHostname, 0, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelUsername, 1, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueUsername, 1, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelOperatingSystem, 2, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueOperatingSystem, 2, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelDistro, 3, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueDistro, 3, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelCpu, 4, 0, Qt::AlignTop | Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueCpu, 4, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelMemory, 5, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueMemory, 5, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSSHCommand, 6, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSSHCommand, 6, 1, Qt::AlignLeft);
    this->gridLayout->addWidget(this->labelSCPCommand, 7, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSCPCommand, 7, 1, Qt::AlignLeft);

    this->gridLayout->addWidget(this->labelSCPDirCommand, 8, 0, Qt::AlignLeft);
    this->gridLayout->addWidget(this->valueSCPDirCommand, 8, 1, Qt::AlignLeft);

    this->gridLayout->setRowStretch(9, 1);
    this->gridLayout->setColumnStretch(3, 1);

    this->page->setLayout(this->gridLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(this->page);

    this->widgetStack->addWidget(this->disabledWidget);
    this->widgetStack->addWidget(scrollArea);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->widgetStack);
    this->setLayout(layout);
}

void MachineInfoWidget::setMachineEnabled(bool enabled)
{
    this->enabled = enabled;

    if (this->enabled) {
        this->widgetStack->setCurrentIndex(1);
    } else {
        this->widgetStack->setCurrentIndex(0);
    }
}

void MachineInfoWidget::updateData(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;

    this->valueHostname->setText(connEntry->hostname);
    this->valueUsername->setText(connEntry->username);

    this->valueSSHCommand->setText(connEntry->generateSSHCommand());
    this->valueSCPCommand->setText(connEntry->generateSCPCommand("File2Copy", "/tmp"));
    this->valueSCPDirCommand->setText(connEntry->generateSCPCommand("Dir2Copy", "/tmp", true));

    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "uname -s -r -i", this, "sshResultReceived");
}

void MachineInfoWidget::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
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
        this->valueMemory->setText(mem);
    } else if (cmdResult->command.startsWith("grep 'model name' ")) {
        QString cpu = cmdResult->resultString.remove("model name").remove(": ").trimmed();
        cpu = cpu.replace(QRegExp("\n\\s+"), "\n");
        this->valueCpu->setText(cpu);
    }
}
