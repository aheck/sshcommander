#include "SSHTermWidget.h"

SSHTermWidget::SSHTermWidget(const QStringList *args, std::weak_ptr<SSHConnectionEntry> connEntryWeak, QWidget *parent)
    : QTermWidget(0, parent)
{
    this->connEntryWeak = connEntryWeak;
    auto connEntry = this->connEntryWeak.lock();

    if (!connEntry->password.isEmpty()) {
        connect(this, SIGNAL(receivedData(QString)), this, SLOT(dataReceived(QString)));
    }

    Preferences &preferences = Preferences::getInstance();

    this->setAutoClose(false);
    this->setShellProgram("/usr/bin/ssh");
    this->setArgs(*args);
    this->setTerminalFont(preferences.getTerminalFont());

    this->setColorScheme(preferences.getColorScheme());
    this->setScrollBarPosition(QTermWidget::ScrollBarRight);
}

void SSHTermWidget::dataReceived(const QString &text)
{
    auto connEntry = this->connEntryWeak.lock();

    if (!connEntry) {
        std::cerr << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__ << std::endl;
        return;
    }

    if (connEntry == nullptr) {
        return;
    }

    if (connEntry->password.isEmpty()) {
        return;
    }

    this->passwordLineCounter++;

    if (this->passwordLineCounter > 10) {
        disconnect(this, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }

    std::cout << text.toStdString() << "\n";

    if (QRegExp("^.*( )?(p|P)assword:( )?$").exactMatch(text) || QRegExp("Password for \\S+@\\S+:").exactMatch(text)) {
        std::cout << "Sending ssh password...\n";
        this->sendText(connEntry->password + "\n");

        disconnect(this, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }
}
