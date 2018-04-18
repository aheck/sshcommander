/*****************************************************************************
 *
 *  AppletWidget is the widget that instantiates and contains all the applets
 *  in the lower part of the screen.
 *
 ****************************************************************************/

#ifndef APPLETWIDGET_H
#define APPLETWIDGET_H

#include <QHBoxLayout>
#include <QTabWidget>

#include "applets/AWSInfoApplet.h"
#include "applets/MachineInfoApplet.h"
#include "applets/NotesEditor.h"
#include "applets/PortsApplet.h"
#include "applets/ProcessesApplet.h"
#include "applets/RoutesApplet.h"
#include "applets/SSHFilesystemApplet.h"
#include "applets/TunnelsApplet.h"
#include "InactiveSessionWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"

class AppletWidget : public QWidget
{
    Q_OBJECT

public:
    AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);
    ~AppletWidget();

public slots:
    void appletChanged(int index);

private:
    QList<Applet*> applets;
    QTabWidget *appletTab;
};

#endif
