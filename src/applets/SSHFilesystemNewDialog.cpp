#include "SSHFilesystemNewDialog.h"

SSHFilesystemNewDialog::SSHFilesystemNewDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(500);
    this->setMinimumHeight(180);
    this->setMaximumHeight(180);
    this->connEntry = nullptr;

    this->setWindowTitle(tr("Mount remote SFTP directory..."));
    this->setWindowIcon(QIcon(":/images/applications-internet.svg"));

    this->localDirLineEdit = new QLineEdit();
    this->remoteDirLineEdit = new QLineEdit();
    this->shortDescriptionLineEdit = new QLineEdit();

    QHBoxLayout *localDirLayout = new QHBoxLayout();
    localDirLayout->addWidget(this->localDirLineEdit);
    QToolButton *localDirButton = new QToolButton();
    localDirButton->setText("...");
    connect(localDirButton, SIGNAL(clicked()), this, SLOT(selectLocalDir()));
    localDirLayout->addWidget(localDirButton);

    QHBoxLayout *remoteDirLayout = new QHBoxLayout();
    remoteDirLayout->addWidget(this->remoteDirLineEdit);
    QToolButton *remoteDirButton = new QToolButton();
    remoteDirButton->setText("...");
    connect(remoteDirButton, SIGNAL(clicked()), this, SLOT(selectRemoteDir()));
    remoteDirLayout->addWidget(remoteDirButton);

    this->formLayout = new QFormLayout;
    this->formLayout->addRow(tr("Local Directory:"), localDirLayout);
    this->formLayout->addRow(tr("Remote Directory:"), remoteDirLayout);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);

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
    mainLayout->addLayout(this->formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
}

void SSHFilesystemNewDialog::setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;
}

void SSHFilesystemNewDialog::acceptDialog()
{
    /*
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    */

    this->accept();
}

const QString SSHFilesystemNewDialog::getLocalDir()
{
    return this->localDirLineEdit->text();
}

const QString SSHFilesystemNewDialog::getRemoteDir()
{
    return this->remoteDirLineEdit->text();
}

const QString SSHFilesystemNewDialog::getShortDescription()
{
    return this->shortDescriptionLineEdit->text();
}

void SSHFilesystemNewDialog::clear()
{
    this->localDirLineEdit->clear();
    this->remoteDirLineEdit->clear();
    this->shortDescriptionLineEdit->clear();
}

void SSHFilesystemNewDialog::selectLocalDir()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    if (dialog.exec()) {
        QStringList filenames = dialog.selectedFiles();
        this->localDirLineEdit->setText(filenames.first());
    }
}

void SSHFilesystemNewDialog::selectRemoteDir()
{
    SFTPDirectoryDialog dialog(this);
    dialog.setConnEntry(this->connEntry);

    if (dialog.exec()) {
        this->remoteDirLineEdit->setText(dialog.getSelectedPath());
    }
}
