/*****************************************************************************
 *
 * Applet is the base class for all Applets.
 *
 ****************************************************************************/

#ifndef APPLET_H
#define APPLET_H

#include <memory>

#include <QWidget>

#include "AWSInstance.h"
#include "SSHConnectionEntry.h"

class Applet : public QWidget
{
    Q_OBJECT

public:
    Applet();
    ~Applet();

    virtual const QString getDisplayName();
    virtual QIcon getIcon();

    virtual void init(std::shared_ptr<SSHConnectionEntry> connEntry);
    virtual void onShow();

protected:
    std::shared_ptr<SSHConnectionEntry> connEntry;

private:
    bool firstShow;
};

#endif
