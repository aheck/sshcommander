#include "Applet.h"

Applet::Applet()
{

}

Applet::~Applet()
{

}

const QString Applet::getDisplayName()
{
    return "Applet";
}

QIcon Applet::getIcon()
{
    return QIcon();
}

void Applet::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    this->connEntry = connEntry;
}
