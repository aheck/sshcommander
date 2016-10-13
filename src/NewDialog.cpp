#include "NewDialog.h"

NewDialog::NewDialog(MainWindow *mainWindow)
{
    this->mainWindow = mainWindow;

    hostnameLineEdit = new QLineEdit();
    usernameLineEdit = new QLineEdit();
    sshkeyLineEdit = new QLineEdit();

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(sshkeyLineEdit);
    QPushButton *fileButton = new QPushButton("...");
    fileButton->setDefault(false);
    QObject::connect(fileButton, SIGNAL (clicked()), this, SLOT (selectKeyFile()));
    horizontalLayout->addWidget(fileButton);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Hostname:"), hostnameLineEdit);
    formLayout->addRow(tr("Username:"), usernameLineEdit);
    formLayout->addRow(tr("SSH Key:"), horizontalLayout);

    QPushButton *connectButton = new QPushButton(tr("Connect"));
    QObject::connect(connectButton, SIGNAL (clicked()), this, SLOT (accept()));
    connectButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, SIGNAL (clicked()), this, SLOT(reject()));
    cancelButton->setDefault(false);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(connectButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
}

NewDialog::~NewDialog()
{
}

void NewDialog::selectKeyFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open SSH Key File"),
            QString("%1/.ssh").arg(QDir::homePath()),
            tr("All Files (*)"));

    if (!filename.isEmpty()) {
        sshkeyLineEdit->setText(filename);
    }
}
