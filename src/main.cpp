#include <QApplication>

#include "MainWindow.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName(PROGRAM_NAME);

    // style sheets
    app.setStyleSheet(
            "QScrollArea QWidget#scrollAreaContent {background-color: white;}"
            "QLineEdit {background-color: white;}"
            "QLineEdit::focus {border: 3px solid #c1e0ff; border-radius: 5px;}"
            "QComboBox {background-color: white; selection-background-color: #4a90d9;}"
            "QComboBox:editable::focus {border: 3px solid #c1e0ff; border-radius: 5px;}"
            "QSplitter::handle:horizontal {width: 3px; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 grey, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
            "QSplitter#terminalSplitter::handle   {height: 1em; background: #303030}"
            "QGroupBox {border: 1px solid grey; margin-top: 0.5em; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, stop: 0 #f4f4f4, stop: 1 #fafafa);}"
            "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; border: 1px solid grey;}"
    );

    // application-wide settings for QSettings
    QCoreApplication::setOrganizationName("aheck");
    QCoreApplication::setOrganizationDomain("github.com/aheck");
    QCoreApplication::setApplicationName("SSH Commander");

    MainWindow mainWindow;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &mainWindow, SLOT(aboutToQuit()));
    mainWindow.show();

    return app.exec();
}
