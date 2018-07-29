/*****************************************************************************
 *
 * CustomTabBar is a specialization of QTabBar which implements a mechanism
 * for signaling that the contents of a tab which is not currently active
 * have changed by changing the text color of the tab label.
 *
 * Call setTabChanged() with the index of the tab you want to bring to the
 * users attention.
 *
 * If it is not the active tab its text color will change to blue and revert
 * to black once the user clicks on it. Calls of setTabChanged() for the
 * active tab will be ignored.
 *
 ****************************************************************************/

#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

class CustomTabBar : public QTabBar
{
    Q_OBJECT

public:
    CustomTabBar(QWidget *parent = 0);
    void setTabChanged(int index);

private slots:
    void currentChanged(int index);
};

#endif
