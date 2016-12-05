#include <QApplication>

#include "MainWindow.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName(PROGRAM_NAME);

    // style sheets
    app.setStyleSheet(
            "QSplitter::handle:vertical   {height: 3px; background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #fafafa, stop: 0.4 grey, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
            "QSplitter::handle:horizontal {width:  3px; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 grey, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
            );

    // application-wide settings for QSettings
    QCoreApplication::setOrganizationName("aheck");
    QCoreApplication::setOrganizationDomain("github.com/aheck");
    QCoreApplication::setApplicationName("sshcommander");

    MainWindow mainWindow;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &mainWindow, SLOT(aboutToQuit()));
    mainWindow.show();

    return app.exec();
}
