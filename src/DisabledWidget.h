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

private:
    QLabel *disabledLabel;
};

#endif
