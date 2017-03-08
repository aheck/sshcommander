#include "AWSConsoleWindow.h"

AWSConsoleWindow::AWSConsoleWindow(QWidget *parent)
{
    this->setLayout(new QVBoxLayout());
    this->setWindowTitle(QString("AWS Console - ") + PROGRAM_NAME);
}

void AWSConsoleWindow::closeEvent(QCloseEvent *event)
{
    emit requestReattach();
    event->ignore();
}
