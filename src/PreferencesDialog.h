#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
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

#include "qtermwidget.h"

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog();

    QFont getFont();
    void setFont(const QFont &font);
    const QString getAWSAccessKey();
    void setAWSAccessKey(const QString accessKey);
    const QString getAWSSecretKey();
    void setAWSSecretKey(const QString secretKey);
    const QString getColorScheme();
    void setColorScheme(const QString colorScheme);

public slots:
    void selectFont();

private:
    QTabWidget *tabs;
    QFont terminalFont;
    QPushButton *fontButton;
    QString accessKey;
    QString secretKey;
    QLineEdit *accessKeyLineEdit;
    QLineEdit *secretKeyLineEdit;
    QComboBox *colorSchemeComboBox;

    bool newFontSelected;
    QFont newFont;
};

#endif
