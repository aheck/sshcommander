#ifndef MACHINEINFOWIDGET_H
#define MACHINEINFOWIDGET_H

#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class MachineInfoWidget : public QWidget
{
    Q_OBJECT

public:
    MachineInfoWidget();
    ~MachineInfoWidget();

    void setHostname(const QString hostname);
    void setUsername(const QString username);
    void setMachineEnabled(bool enabled);

private:
    bool enabled;
    QWidget *page;
    QWidget *disabledPage;
    QStackedWidget *widgetStack;
    QGridLayout *gridLayout;
    QLabel *labelHostname;
    QLabel *valueHostname;
    QLabel *labelUsername;
    QLabel *valueUsername;
};

#endif
