#include "NotificationManager.h"

NotificationManager& NotificationManager::getInstance()
{
    static NotificationManager notificationManager;
    return notificationManager;
}

void NotificationManager::init(QWidget *mainWindow)
{
    NotificationManager::getInstance().initObject(mainWindow);
}

NotificationManager::NotificationManager()
{
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(update()));
    this->timer.setInterval(100);
    this->ticksSincePopupAppeared = 0;
}

NotificationManager::~NotificationManager()
{
}

void NotificationManager::addNotification(QString svgIconPath, QString message)
{
    NotificationEntry entry;

    entry.svgIconPath = svgIconPath;
    entry.message = message;

    this->queue.enqueue(entry);

    this->update(true);
}

void NotificationManager::update(bool nonTimerCall)
{
    if (nonTimerCall == false) {
        this->ticksSincePopupAppeared++;
    }

    if (this->ticksSincePopupAppeared > 19) {
        this->popup->hide();
    }

    if (this->queue.isEmpty()) {
        // nothing to do, shut everything down until the next notification is
        // added by a user of this singleton
        if (this->popup->isHidden()) {
            this->timer.stop();
            this->ticksSincePopupAppeared = 0;
        }

        return;
    }

    if (!this->timer.isActive()) {
        this->timer.start();
    }

    if (this->popup->isHidden()) {
        NotificationEntry entry = this->queue.dequeue();

        this->popup->setContent(entry.svgIconPath, entry.message);
        this->popup->show();
        this->ticksSincePopupAppeared = 0;
    }
}

void NotificationManager::initObject(QWidget *mainWindow)
{
    this->popup = new NotificationPopup(mainWindow);
}
