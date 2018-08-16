/*****************************************************************************
 *
 * NotificationManager is a singleton that manages the queuing and display
 * of all in-app notifications.
 *
 ****************************************************************************/

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QQueue>
#include <QTimer>

#include "NotificationPopup.h"

struct NotificationEntry
{
    QString svgIconPath;
    QString message;
};

class NotificationManager : public QObject
{
    Q_OBJECT

public:
    static NotificationManager& getInstance();
    static void init(QWidget *mainWindow);

    NotificationManager(NotificationManager const &other) = delete;
    void operator=(NotificationManager const &) = delete;

    void addNotification(QString svgIconPath, QString message);

private slots:
    void update(bool nonTimerCall = false);

private:
    NotificationManager();
    ~NotificationManager();
    void initObject(QWidget *mainWindow);

    NotificationPopup *popup;
    QQueue<NotificationEntry> queue;
    QTimer timer;
    unsigned int ticksSincePopupAppeared;
};

#endif
