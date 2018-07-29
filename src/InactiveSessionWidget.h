/*****************************************************************************
 *
 * InactiveSessionWidget is the widget which is shown instead of a terminal
 * if the terminal session is not connected, yet.
 *
 * It shows a message informing the user about the current state of this
 * session and it provides a button to connect the session.
 *
 ****************************************************************************/

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
