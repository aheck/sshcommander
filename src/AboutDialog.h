/*****************************************************************************
 *
 * AboutDialog implements the dialog that is shown when you go to
 * "Help/About" in the program menu.
 *
 ****************************************************************************/

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include <libssh2.h>

#include "git_commit_id.h"
#include "globals.h"

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = nullptr);
};

#endif
