#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "git_commit_id.h"
#include "globals.h"

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog();
};

#endif
