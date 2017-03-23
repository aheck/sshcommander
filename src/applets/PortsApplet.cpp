#include "PortsApplet.h"

PortsApplet::PortsApplet()
{
    this->toolBar = new QToolBar();
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"),
            "Reload", this, SLOT(reloadData()));
    this->toolBar->setOrientation(Qt::Vertical);

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->toolBar);

    this->table = new QTableWidget(this);
    QStringList columnNames = {"Protocol", "Local Address", "Local Port", "Foreign Address", "Foreign Port", "State"};
    this->table->setColumnCount(6);
    this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->table->verticalHeader()->setVisible(false);
    this->table->setHorizontalHeaderLabels(columnNames);
    this->table->horizontalHeader()->setStretchLastSection(true);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    for (int i = 0; i < this->table->horizontalHeader()->count(); i++) {
        this->table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
    }
    this->layout()->addWidget(this->table);
}

const QString PortsApplet::getDisplayName()
{
    return tr("Ports");
}

QIcon PortsApplet::getIcon()
{
    return QIcon(":/images/network-wired.svg");
}

void PortsApplet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);
    this->onFirstShow();
}

void PortsApplet::onFirstShow()
{
    this->updateData();
}

void PortsApplet::updateData()
{
    SSHConnectionManager &connMgr = SSHConnectionManager::getInstance();
    connMgr.executeRemoteCmd(this->connEntry, "netstat -lntu", this, "sshResultReceived");
}

void PortsApplet::sshResultReceived(std::shared_ptr<RemoteCmdResult> cmdResult)
{
    if (!cmdResult->isSuccess) {
        std::cout << "ERROR: SSH remote command failed: " << cmdResult->errorString.toStdString() << std::endl;
        return;
    }

    QStringList lines = cmdResult->resultString.trimmed().split("\n");

    // remove the two header lines
    if (lines.count() > 2) {
        lines.removeFirst();
        lines.removeFirst();
    }

    QRegularExpression spaceRegExp("\\s+");
    QRegularExpression hostnameRegExp("^(.*):(\\d+|\\*)$");
    this->table->setRowCount(lines.count());

    for (int i = 0; i < lines.count(); i++) {
        QString line = lines.at(i);

        QStringList fields = line.split(spaceRegExp);

        if (fields.count() < 5) {
            continue;
        }

        // protocol
        this->table->setItem(i, 0, new QTableWidgetItem(fields.at(0)));

        // local address and local port
        QString localAddressAndPort = fields.at(3);

        QRegularExpressionMatch match = hostnameRegExp.match(localAddressAndPort);
        if (match.hasMatch()) {
            this->table->setItem(i, 1, new QTableWidgetItem(match.captured(1)));
            this->table->setItem(i, 2, new QTableWidgetItem(match.captured(2)));
        }

        // foreign address and foreign port
        QString foreignAddressAndPort = fields.at(4);

        match = hostnameRegExp.match(foreignAddressAndPort);
        if (match.hasMatch()) {
            this->table->setItem(i, 3, new QTableWidgetItem(match.captured(1)));
            this->table->setItem(i, 4, new QTableWidgetItem(match.captured(2)));
        }

        if (fields.count() >= 6) {
            this->table->setItem(i, 5, new QTableWidgetItem(fields.at(5)));
        }
    }
}

void PortsApplet::reloadData()
{
    this->updateData();
}
