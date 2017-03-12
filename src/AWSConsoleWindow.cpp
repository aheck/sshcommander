#include "AWSConsoleWindow.h"

AWSConsoleWindow::AWSConsoleWindow(QWidget *parent)
{
    this->setLayout(new QVBoxLayout());
    this->setWindowTitle(QString("AWS Console - ") + PROGRAM_NAME);
    this->resize(1024, 768);
}

void AWSConsoleWindow::closeEvent(QCloseEvent *event)
{
    emit requestReattach();
    event->ignore();
}
