#include "TerminalContainer.h"

#include <iostream>

TerminalContainer::TerminalContainer(QUuid uuid, QWidget *parent) :
        QWidget(parent)
{
    this->widget = nullptr;
    this->uuid = uuid;

    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
}

TerminalContainer::~TerminalContainer()
{
    if (this->widget != nullptr) {
        delete this->widget;
    }
}

QUuid TerminalContainer::getUuid()
{
    return this->uuid;
}

void TerminalContainer::setWidget(QWidget *widget)
{
    if (this->widget == widget) {
        return;
    }

    QLayoutItem *child = this->layout()->takeAt(0);
    if (child != nullptr) {
        //delete child;
    }

    this->widget = widget;
    this->layout()->addWidget(this->widget);
}

QWidget* TerminalContainer::getWidget()
{
    return this->widget;
}

QString TerminalContainer::getWidgetClassname()
{
    if (this->widget == nullptr) {
        return "";
    }

    return this->widget->metaObject()->className();
}
