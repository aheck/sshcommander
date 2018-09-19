#include <QApplication>
#include <QDir>
#include <QFile>
#include <QLockFile>

#include "MainWindow.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName(PROGRAM_NAME);

    // application-wide settings for QSettings
    QCoreApplication::setOrganizationName("aheck");
    QCoreApplication::setOrganizationDomain("github.com/aheck");
    QCoreApplication::setApplicationName("SSH Commander");

#ifdef Q_OS_MACOS
    app.setStyle("macintosh");
#endif

    QDir appConfigPath = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (!appConfigPath.exists()) {
        appConfigPath.mkpath(appConfigPath.absolutePath());
    }

    QString lockFilePath = appConfigPath.filePath("appRunning.lock");
    QLockFile lockFile(lockFilePath);
    QFile notifyFile(QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath("appNotify"));

    if (!lockFile.tryLock(100)) {
        // signal other instance of SSH Commander to bring the MainWindow to the
        // foreground
        notifyFile.open(QIODevice::WriteOnly);
        notifyFile.close();

        return 1;
    }

    // initially create the notify file on normal startup
    notifyFile.open(QIODevice::WriteOnly);
    notifyFile.close();

    // style sheets
    app.setStyleSheet(
            "QScrollArea QWidget#scrollAreaContent {background-color: white;}"
            "QLineEdit {background-color: white;}"
            "QLineEdit::focus {border: 3px solid #c1e0ff; border-radius: 5px;}"
            "QComboBox {background-color: white; selection-background-color: #4a90d9;}"
            "QComboBox:editable::focus {border: 3px solid #c1e0ff; border-radius: 5px;}"
            "QSplitter::mainSplitter:handle {width: 3px; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 grey, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);}"
            "QSplitter#terminalSplitter::handle   {height: 1em; background: #303030}"
            "QGroupBox {border: 1px solid grey; margin-top: 0.5em; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, stop: 0 #f4f4f4, stop: 1 #fafafa);}"
            "QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; border: 1px solid grey;}"
#ifdef Q_OS_MACOS
            "QToolBar {background-color: transparent; border: 0; padding-left: 3px; padding-top: 5px; padding-bottom: 5px;}"
            "QToolBar:vertical {padding-left: 0; padding-right: 0; }"
#endif
    );

    MainWindow mainWindow;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &mainWindow, SLOT(aboutToQuit()));
    mainWindow.show();

    return app.exec();
}
