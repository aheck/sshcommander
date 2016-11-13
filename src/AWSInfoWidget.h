#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#ifndef AWSINFOWIDGET_H
#define AWSINFOWIDGET_H

class AWSInfoWidget : public QWidget
{
    Q_OBJECT

public:
    AWSInfoWidget();
    ~AWSInfoWidget();

    void setInstanceId(const QString instanceId);
    void setRegion(const QString region);

private:
    QGridLayout gridLayout;
    QLabel labelInstanceId;
    QLabel valueInstanceId;
    QLabel labelRegion;
    QLabel valueRegion;
};

#endif
