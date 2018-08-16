#include "NotificationPopup.h"

NotificationPopup::NotificationPopup(QWidget *parent)
    : QWidget(parent, Qt::SplashScreen | Qt::FramelessWindowHint)
{
    this->setLayout(new QHBoxLayout());

    this->svgWidget = new SvgWidget(this);
    svgWidget->setFixedSize(32, 32);
    svgWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->messageLabel = new QLabel(this);

    this->layout()->addWidget(this->svgWidget);
    this->layout()->addWidget(this->messageLabel);
    this->setMinimumWidth(300);
    this->setMaximumWidth(300);
    this->setMinimumHeight(50);
}

NotificationPopup::~NotificationPopup()
{
}

void NotificationPopup::showEvent(QShowEvent *event)
{
    QWidget *parent = static_cast<QWidget*>(this->parent());

    this->move(parent->window()->frameGeometry().topLeft() +
            parent->window()->rect().topRight() -
            rect().topRight() + QPoint(-15, 40));

    QWidget::showEvent(event);
}

void NotificationPopup::setContent(QString svgIconPath, QString message)
{
    this->svgWidget->load(svgIconPath);
    this->messageLabel->setText(message);
}

void NotificationPopup::mouseReleaseEvent(QMouseEvent *event)
{
    this->hide();
}
