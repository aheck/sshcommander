/*****************************************************************************
 *
 * NotificationPopup implements a popup window which informs the user about
 * important events which require his attention but no action.
 *
 ****************************************************************************/

#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <iostream>

#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QWidget>

#include "SvgWidget.h"

class NotificationPopup : public QWidget
{
    Q_OBJECT

public:
    NotificationPopup(QWidget *parent);
    ~NotificationPopup();

    void setContent(QString svgIconPath, QString message);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    SvgWidget *svgWidget;
    QLabel *messageLabel;
};

#endif
