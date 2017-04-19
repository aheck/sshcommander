#include "AppletWidget.h"

AppletWidget::AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent)
{
    this->appletTab = new QTabWidget();

    // create the connection applets
    this->applets.append(new MachineInfoApplet());
    this->applets.append(new NotesEditor());
    if (connEntry->isAwsInstance) {
        this->applets.append(new AWSInfoApplet());
    }
    this->applets.append(new PortsApplet());

    // create the tab where the applets reside
    this->appletTab = new QTabWidget();

    for (auto applet : this->applets) {
        this->appletTab->addTab(applet, applet->getIcon(), applet->getDisplayName());
        applet->init(connEntry);
    }

    connect(this->appletTab, SIGNAL(currentChanged(int)), this, SLOT(appletChanged(int)));

    this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->appletTab);
}

AppletWidget::~AppletWidget()
{

}

void AppletWidget::appletChanged(int index)
{
    Applet *applet = static_cast<Applet*>(this->appletTab->currentWidget());
    applet->onShow();
}
