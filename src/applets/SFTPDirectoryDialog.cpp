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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("OK"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(this->remoteFileBrowser);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

void SFTPDirectoryDialog::setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;
    this->remoteFileSystemModel->setConnEntry(connEntry);
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
