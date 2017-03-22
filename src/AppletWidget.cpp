#include "AppletWidget.h"

AppletWidget::AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent)
{
    this->appletTab = new QTabWidget();

    // create the connection applets
    this->applets.append(new MachineInfoWidget());
    this->applets.append(new NotesEditor());
    if (connEntry->isAwsInstance) {
        this->applets.append(new AWSInfoWidget());
    }
    this->applets.append(new PortsApplet());

    // create the tab where the applets reside
    this->appletTab = new QTabWidget();
    for (auto applet : this->applets) {
        this->appletTab->addTab(applet, applet->getIcon(), applet->getDisplayName());
        applet->init(connEntry);
    }

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->appletTab);
}

AppletWidget::~AppletWidget()
{

}
