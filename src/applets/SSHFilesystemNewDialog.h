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

public slots:
    void acceptDialog();

protected slots:
    void selectLocalDir();

private:
    QFormLayout *formLayout;
    QLineEdit *localDirLineEdit;
    QLineEdit *remoteDirLineEdit;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
