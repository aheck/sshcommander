/*****************************************************************************
 *
 *  AppletWidget is the widget that instantiates and contains all the applets
 *  in the lower part of the screen.
 *
 ****************************************************************************/

#ifndef APPLETWIDGET_H
#define APPLETWIDGET_H

#include <QHash>
#include <QHBoxLayout>
#include <QTabWidget>

#include "applets/AWSInfoApplet.h"
#include "applets/FileBrowserApplet.h"
#include "applets/FileTransfersApplet.h"
#include "applets/MachineInfoApplet.h"
#include "applets/NotesEditor.h"
#include "applets/PortsApplet.h"
#include "applets/ProcessesApplet.h"
#include "applets/RoutesApplet.h"
#include "applets/SSHFilesystemApplet.h"
#include "applets/TunnelsApplet.h"

#include "CustomTabBar.h"
#include "InactiveSessionWidget.h"
#include "Preferences.h"
#include "SSHConnectionEntry.h"

class AppletWidget : public QTabWidget
{
    Q_OBJECT

public:
    AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent = 0);
    ~AppletWidget();

public slots:
    void appletChanged(int index);

private slots:
    void appletContentChanged();

private:
    QList<Applet*> applets;
    QHash<QWidget*, int> appletToIndex;
    CustomTabBar *tabBar;
};

#endif
