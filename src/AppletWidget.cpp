#include "AppletWidget.h"

AppletWidget::AppletWidget(std::shared_ptr<SSHConnectionEntry> connEntry, QWidget *parent)
{
    this->tabBar = new CustomTabBar();
    this->setTabBar(tabBar);

    // create the connection applets
    this->applets.append(new MachineInfoApplet());
    this->applets.append(new NotesEditor());

    FileBrowserApplet *fileBrowserApplet = new FileBrowserApplet();
    FileTransfersApplet *fileTransfersApplet = new FileTransfersApplet();
    fileBrowserApplet->setFileTransfersApplet(fileTransfersApplet);

    this->applets.append(fileBrowserApplet);
    this->applets.append(fileTransfersApplet);
    if (connEntry->isAwsInstance) {
        this->applets.append(new AWSInfoApplet());
    }
    this->applets.append(new PortsApplet());
    this->applets.append(new TunnelsApplet());
    this->applets.append(new RoutesApplet());
    this->applets.append(new ProcessesApplet());
    this->applets.append(new SSHFilesystemApplet());

    for (auto applet : this->applets) {
        this->addTab(applet, applet->getIcon(), applet->getDisplayName());
        applet->init(connEntry);

        connect(applet, SIGNAL(changed()), this, SLOT(appletContentChanged()));
    }

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(appletChanged(int)));

    //this->layout()->setContentsMargins(0, 0, 0, 0);
}

AppletWidget::~AppletWidget()
{

}

void AppletWidget::appletChanged(int index)
{
    Applet *applet = static_cast<Applet*>(this->currentWidget());

    if (applet == nullptr) {
        return;
    }

    applet->onShow();
}

void AppletWidget::appletContentChanged()
{
    int index = this->applets.indexOf(static_cast<Applet*>(sender()));

    if (index < 0) {
        return;
    }

    if (this->currentIndex() == index) {
        return;
    }

    this->tabBar->setTabChanged(index);
}
