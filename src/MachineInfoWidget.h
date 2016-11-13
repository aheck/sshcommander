#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

class MachineInfoWidget : public QWidget
{
    Q_OBJECT

public:
    MachineInfoWidget();
    ~MachineInfoWidget();

    void setHostname(const QString hostname);
    void setUsername(const QString username);

private:
    QGridLayout gridLayout;
    QLabel labelHostname;
    QLabel valueHostname;
    QLabel labelUsername;
    QLabel valueUsername;
};

#endif
