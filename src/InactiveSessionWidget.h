#ifndef INACTIVESESSIONWIDGET_H
#define INACTIVESESSIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

class InactiveSessionWidget : public QWidget
{
    Q_OBJECT

signals:
    void createSession(QUuid uuid);

public:
    InactiveSessionWidget(QUuid uuid);

private slots:
    void createSessionSlot();

private:
    QUuid uuid;
};

#endif
