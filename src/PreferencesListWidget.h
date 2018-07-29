/*****************************************************************************
 *
 * PreferencesListWidget is the widget used to show the list of clickable
 * category icons in the preferences dialog.
 *
 ****************************************************************************/

#ifndef PREFERENCESLISTWIDGET_H
#define PREFERENCESLISTWIDGET_H

#include <QListWidget>
#include <QSize>

class PreferencesListWidget : public QListWidget
{
    Q_OBJECT

public:
    PreferencesListWidget(QWidget *parent = 0);

    QSize sizeHint() const override;
};

#endif
