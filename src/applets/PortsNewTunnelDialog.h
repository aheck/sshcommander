#ifndef PORTSNEWTUNNELDIALOG_H
#define PORTSNEWTUNNELDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
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

public slots:
    void acceptDialog();

private:
    int localPort;
    int remotePort;
    QFormLayout *formLayout;
    QLineEdit *localPortLineEdit;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
