/*****************************************************************************
 *
 * DetachedSessionWidget is the widget which is shown where the ssh terminal
 * was if the user detaches it.
 *
 * It tells the user that this session was detached to its own window and
 * provides a button to show the window.
 *
 ****************************************************************************/

#ifndef DETACHEDSESSIONWIDGET_H
#define DETACHEDSESSIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

class DetachedSessionWidget : public QWidget
{
    Q_OBJECT

public:
    DetachedSessionWidget();
    void setUuid(QUuid uuid);

signals:
    void requestShowWindow(QUuid uuid);

private slots:
    void showDetachedWindow();

private:
    QUuid uuid;
    QWidget *detachedWindow;
};

#endif
