/*****************************************************************************
 *
 * SFTPDirectoryDialog is the dialog used to select a directory on a remote
 * machine via SFTP.
 *
 ****************************************************************************/

#ifndef SFTPDIRECTORYDIALOG_H
#define SFTPDIRECTORYDIALOG_H

#include <iostream>

#include <QComboBox>
#include <QDialog>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTreeView>
#include <QVBoxLayout>

#include "SFTPFilesystemModel.h"

class SFTPDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    SFTPDirectoryDialog(QWidget *parent = Q_NULLPTR);

    void setConnEntry(std::shared_ptr<SSHConnectionEntry> connEntry);
    QString getSelectedPath();

private:
    std::shared_ptr<SSHConnectionEntry> connEntry;
    QPushButton *okButton;
    QPushButton *cancelButton;
    SFTPFilesystemModel *remoteFileSystemModel;
    QTreeView *remoteFileBrowser;
};

#endif
