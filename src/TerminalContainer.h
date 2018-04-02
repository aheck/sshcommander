/*****************************************************************************
 *
 * TerminalContainer is the widget that contains a terminal widget or its
 * placeholder widgets (e.g. for telling the user that the session is inactive)
 *
 ****************************************************************************/

#ifndef TERMINALCONTAINER_H
#define TERMINALCONTAINER_H

#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

class TerminalContainer : public QWidget
{
    Q_OBJECT

public:
    TerminalContainer(QUuid uuid, QWidget *parent = 0);
    ~TerminalContainer();

    QUuid getUuid();
    void setWidget(QWidget *widget);
    QWidget* getWidget();
    QString getWidgetClassname();
    void setDetached(bool detached);
    bool isDetached();

private:
    QUuid uuid;
    bool detached;
    QWidget *widget;
};

#endif
