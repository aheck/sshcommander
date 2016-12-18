#ifndef NOTESEDITOR_H
#define NOTESEDITOR_H

#include <iostream>
#include <memory>

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QFontComboBox>
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
    void changeFont(const QFont &font);
    void changeFontSize(const QString fontSize);
    void formatTextBold();
    void formatTextItalic();
    void formatTextUnderline();
    void selectTextColor();
    void selectBackgroundColor();
    void colorChanged(const QColor &color);
    void backgroundColorChanged(const QColor &color);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void fontChanged(const QFont &font);

signals:
    void textChanged();

private:
    void applyFormat(const QTextCharFormat &format);

    DisabledWidget *disabledWidget;
    QStackedWidget *widgetStack;
    QTextEdit *editor;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionTextBold;
    QAction *actionTextItalic;
    QAction *actionTextUnderline;
    QAction *actionTextColor;
    QAction *actionTextBackgroundColor;
};

#endif
