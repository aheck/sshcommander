#include "SvgWidget.h"

SvgWidget::SvgWidget(QWidget *parent)
    : QSvgWidget(parent)
{
    this->init();
}

SvgWidget::SvgWidget(const QString &file, QWidget *parent)
    : QSvgWidget(file, parent)
{
    this->init();
}

void SvgWidget::setFixedSize(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
}

QSize SvgWidget::sizeHint() const
{
    if (this->width == 0 || this->height == 0) {
        return QSvgWidget::sizeHint();
    }

    return QSize(this->width, this->height);
}

void SvgWidget::init()
{
    this->width = 0;
    this->height = 0;
}
