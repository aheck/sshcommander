#ifndef NOTESEDITOR_H
#define NOTESEDITOR_H

#include <memory>

#include <QAction>
#include <QColor>
#include <QColorDialog>
#include <QPaintEngine>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QWidget>

#include "DisabledWidget.h"

class NotesEditor : public QWidget
{
    Q_OBJECT

public:
    NotesEditor();

    void setEnabled(bool enabled);
    void setHtml(const QString &text);
    QString toHtml() const;

private slots:
    void textWasChanged();
    void formatTextBold();
    void formatTextItalic();
    void formatTextUnderline();
    void selectTextColor();
    void colorChanged(const QColor &color);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void fontChanged(const QFont &font);

signals:
    void textChanged();

private:
    void applyFormat(const QTextCharFormat &format);
    QPixmap generateCharIcon(char character, QFont font);

    DisabledWidget *disabledWidget;
    QStackedWidget *widgetStack;
    QTextEdit *editor;
    QAction *actionTextBold;
    QAction *actionTextItalic;
    QAction *actionTextUnderline;
    QAction *actionTextColor;
};

#endif
