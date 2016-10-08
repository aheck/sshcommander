#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("SSH Commander");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
