#include "PreferencesListWidget.h"

PreferencesListWidget::PreferencesListWidget(QWidget *parent)
    : QListWidget(parent)
{
}

QSize PreferencesListWidget::sizeHint() const
{
    QSize size = QListWidget::sizeHint();
    size.setHeight(64);

    return size;
}
