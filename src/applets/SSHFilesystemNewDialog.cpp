#include "SSHFilesystemNewDialog.h"

SSHFilesystemNewDialog::SSHFilesystemNewDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle(tr("New SSH Tunnel..."));
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

    this->formLayout = new QFormLayout;
    this->formLayout->addRow(tr("Local Directory:"), localDirLayout);
    this->formLayout->addRow(tr("Remote Directory:"), this->remoteDirLineEdit);
    this->formLayout->addRow(tr("Short Description:"), this->shortDescriptionLineEdit);

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

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
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
