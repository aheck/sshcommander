/*****************************************************************************
 *
 * DisabledWidget is the widget which is shown on the right-hand side if
 * the user has not created any SSH connections, yet.
 *
 ****************************************************************************/

#ifndef DISABLEDWIDGET_H
#define DISABLEDWIDGET_H

#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class DisabledWidget : public QWidget
{
    Q_OBJECT

public:
    DisabledWidget(QString text);

    void setText(QString text);
    void setDescription(QString description);

private:
    QLabel *disabledLabel;
    QLabel *descriptionLabel;
};

#endif
