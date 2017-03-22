/*****************************************************************************
 *
 *  AppletWidget the widget that organizes multiple QTermWidget
 *
 ****************************************************************************/

#ifndef APPLETWIDGET_H
#define APPLETWIDGET_H

#include <QHBoxLayout>
#include <QTabWidget>

#include "applets/AWSInfoWidget.h"
#include "applets/MachineInfoWidget.h"
#include "applets/NotesEditor.h"
#include "applets/PortsApplet.h"
#include "InactiveSessionWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"

class AppletWidget : public QWidget
{
    Q_OBJECT

public:
    AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);
    ~AppletWidget();

private:
    QList<Applet*> applets;
    QTabWidget *appletTab;
};

#endif
