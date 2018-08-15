/*****************************************************************************
 *
 * SvgWidget is a custom extension of QSvgWidget.
 *
 ****************************************************************************/

#ifndef SVGWIDGET_H
#define SVGWIDGET_H

#include <QSvgWidget>

class SvgWidget : public QSvgWidget
{
    Q_OBJECT

public:
    SvgWidget(QWidget *parent = nullptr);
    SvgWidget(const QString &file, QWidget *parent = nullptr);

    void setFixedSize(unsigned int width, unsigned int height);
    QSize sizeHint() const override;

private:
    void init();

    unsigned int width;
    unsigned int height;
};

#endif
