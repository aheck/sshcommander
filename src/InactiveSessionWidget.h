#ifndef INACTIVESESSIONWIDGET_H
#define INACTIVESESSIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class InactiveSessionWidget : public QWidget
{
    Q_OBJECT

signals:
    void createSession();

public:
    InactiveSessionWidget();

private slots:
    void createSessionSlot();
};

#endif
