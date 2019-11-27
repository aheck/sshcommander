#include "SFTPDirectoryDialog.h"

SFTPDirectoryDialog::SFTPDirectoryDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(640);
    this->setMinimumHeight(480);
    this->connEntry = nullptr;

    QVBoxLayout *layout = new QVBoxLayout();

    this->remoteFileBrowser = new QTreeView(this);
    this->remoteFileSystemModel = new SFTPFilesystemModel();
    this->remoteFileSystemModel->setShowOnlyDirs(true);
    this->remoteFileBrowser->setModel(this->remoteFileSystemModel);
    this->remoteFileBrowser->setAllColumnsShowFocus(true);
    this->remoteFileBrowser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    QPushButton *okButton = new QPushButton(tr("OK"));
    connect(okButton, &QPushButton::clicked, this, &SFTPDirectoryDialog::accept);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &SFTPDirectoryDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    layout->addWidget(this->remoteFileBrowser);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

void SFTPDirectoryDialog::setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;
    this->remoteFileSystemModel->setConnEntry(connEntry);

    this->setWindowTitle("Select remote directory on " + connEntry->hostname);
}

QString SFTPDirectoryDialog::getSelectedPath()
{
    QModelIndex cur = this->remoteFileBrowser->currentIndex();

    QString *path = static_cast<QString*>(cur.internalPointer());
    if (path == nullptr) {
        return QString("");
    }

    return *path;
}
