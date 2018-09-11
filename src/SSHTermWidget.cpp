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

    this->passwordRegex1 = QRegExp("^.*( )?(p|P)assword:( )?$");
    this->passwordRegex2 = QRegExp("^Password for \\S+@\\S+:");
}

void SSHTermWidget::dataReceived(const QString &text)
{
    auto connEntry = this->connEntryWeak.lock();

    if (connEntry == nullptr) {
        std::cerr << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__ << std::endl;
        return;
    }

    if (connEntry->password.isEmpty()) {
        disconnect(this, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
        return;
    }

    // Ignore single characters because they are user input sent back because of
    // echo mode.
    if (text.length() == 1) {
        return;
    }

    this->passwordLineCounter++;

    if (this->passwordLineCounter > 10) {
        disconnect(this, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }

    std::cout << text.toStdString() << "\n";

    if (this->passwordRegex1.exactMatch(text) || this->passwordRegex2.exactMatch(text)) {
        std::cout << "Sending ssh password...\n";
        this->sendText(connEntry->password + "\n");

        disconnect(this, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }
}

void SSHTermWidget::updateConsoleSettings(const QFont &font, const QString &colorScheme)
{
    this->setTerminalFont(font);
    this->setColorScheme(colorScheme);
    this->update();
}
