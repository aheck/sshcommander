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
