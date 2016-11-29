#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTabWidget>

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog();

    QFont getFont();
    void setFont(const QFont &font);
    void setFontColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    const QString getAWSAccessKey();
    void setAWSAccessKey(const QString accessKey);
    const QString getAWSSecretKey();
    void setAWSSecretKey(const QString secretKey);

public slots:
    void selectFont();
    void selectFontColor();
    void selectBackgroundColor();

private:
    QTabWidget *tabs;
    QFont terminalFont;
    QPushButton *fontButton;
    QPushButton *fontColorButton;
    QPushButton *backgroundColorButton;
    QColor fontColor;
    QColor backgroundColor;
    QString accessKey;
    QString secretKey;
    QLineEdit *accessKeyLineEdit;
    QLineEdit *secretKeyLineEdit;

    bool newFontSelected;
    QFont newFont;
};

#endif
