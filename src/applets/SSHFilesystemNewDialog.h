#ifndef SSHFILESYSTEMNEWDIALOG_H
#define SSHFILESYSTEMNEWDIALOG_H

#include <memory>

#include <QDialog>
#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QToolButton>

#include "SFTPDirectoryDialog.h"
#include "SSHFilesystemManager.h"

class SSHFilesystemNewDialog : public QDialog
{
    Q_OBJECT

public:
    SSHFilesystemNewDialog(QWidget *parent = Q_NULLPTR);

    const QString getLocalDir();
    const QString getRemoteDir();
    const QString getShortDescription();
    void clear();
    void setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);

public slots:
    void acceptDialog();

protected slots:
    void selectLocalDir();
    void selectRemoteDir();

private:
    std::shared_ptr<SSHConnectionEntry> connEntry;
    QFormLayout *formLayout;
    QLineEdit *localDirLineEdit;
    QLineEdit *remoteDirLineEdit;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
