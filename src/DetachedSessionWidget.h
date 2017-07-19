#ifndef DETACHEDSESSIONWIDGET_H
#define DETACHEDSESSIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QUuid>
#include <QVBoxLayout>
#include <QWidget>

class DetachedSessionWidget : public QWidget
{
    Q_OBJECT

public:
    DetachedSessionWidget();
    void setUuid(QUuid uuid);

signals:
    void requestShowWindow(QUuid uuid);

private slots:
    void showDetachedWindow();

private:
    QUuid uuid;
    QWidget *detachedWindow;
};

#endif
