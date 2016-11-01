#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#ifndef NEWDIALOG_H
#define NEWDIALOG_H

class MainWindow;

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    NewDialog(MainWindow *mainWindow);
    ~NewDialog();

    QLineEdit *hostnameLineEdit;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *sshkeyLineEdit;

public slots:
    void selectKeyFile();
    void acceptDialog();

private:
    MainWindow *mainWindow;
};

#endif
