/*****************************************************************************
 *
 * AWSConsoleWindow is the window which is used when the AWS console is
 * detached to its own window
 *
 ****************************************************************************/

#ifndef AWSCONSOLEWINDOW_H
#define AWSCONSOLEWINDOW_H

#include <QCloseEvent>
#include <QVBoxLayout>
#include <QWidget>

#include "globals.h"

class AWSConsoleWindow : public QWidget
{
    Q_OBJECT

signals:
    void requestReattach();

public:
    AWSConsoleWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif
