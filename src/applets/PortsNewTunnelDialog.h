/*****************************************************************************
 *
 * PortsNewTunnelDialog is a dialog used by PortsApplet to create an SSH
 * tunnel to a remote service.
 *
 ****************************************************************************/

#ifndef PORTSNEWTUNNELDIALOG_H
#define PORTSNEWTUNNELDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QHostInfo>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QToolButton>

#include "TunnelManager.h"

class PortsNewTunnelDialog : public QDialog
{
    Q_OBJECT

public:
    PortsNewTunnelDialog(QWidget *parent = Q_NULLPTR);

    const int getLocalPort();
    const QString getShortDescription();
    void clear();
    void setRemotePort(int remotePort);
    int getRemotePort();
    void setRemoteHostname(QString remoteHostname);
    void update();

public slots:
    void acceptDialog();

private:
    int localPort;
    int remotePort;
    QString remoteHostname;
    QFormLayout *formLayout;
    QLabel *descriptionLabel;
    QLineEdit *localPortLineEdit;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
