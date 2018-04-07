#ifndef TUNNELSNEWDIALOG_H
#define TUNNELSNEWDIALOG_H

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QToolButton>

class TunnelsNewDialog : public QDialog
{
    Q_OBJECT

public:
    TunnelsNewDialog(QWidget *parent = Q_NULLPTR);

    const int getLocalPort();
    const int getRemotePort();
    const QString getShortDescription();
    void clear();

public slots:
    void acceptDialog();

private:
    int localPort;
    int remotePort;
    QFormLayout *formLayout;
    QLineEdit *localPortLineEdit;
    QLineEdit *remotePortLineEdit;
    QLineEdit *shortDescriptionLineEdit;
};

#endif
