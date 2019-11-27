/*****************************************************************************
 *
 * TunnelsNewDialog is a dialog used by TunnelsApplet to create new tunnels.
 *
 ****************************************************************************/

#ifndef TUNNELSNEWDIALOG_H
#define TUNNELSNEWDIALOG_H

#include <memory>

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>

#include "../SvgWidget.h"
#include "../TunnelManager.h"

class TunnelsNewDialog : public QDialog
{
    Q_OBJECT

public:
    TunnelsNewDialog(QWidget *parent = Q_NULLPTR);

    const int getLocalPort();
    void setRemotePort(int remotePort);
    const int getRemotePort();
    const QString getShortDescription();
    void setRemotePortWidgetEnabled(bool value);
    void clear();

public slots:
    void acceptDialog();

private:
    int localPort;
    int remotePort;
    QSpinBox *localPortSpinBox;
    QSpinBox *remotePortSpinBox;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
