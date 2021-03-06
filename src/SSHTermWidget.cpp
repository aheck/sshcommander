#include "SSHTermWidget.h"

SSHTermWidget::SSHTermWidget(const QStringList *args, std::weak_ptr<SSHConnectionEntry> connEntryWeak, QWidget *parent)
    : QTermWidget(0, parent)
{
    this->connEntryWeak = connEntryWeak;
    auto connEntry = this->connEntryWeak.lock();

    if (!connEntry->password.isEmpty()) {
        connect(this, &SSHTermWidget::receivedData, this, &SSHTermWidget::dataReceived);
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
#ifdef Q_OS_MACOS
    // Workaround for macOS paint problem where the terminal is redrawn
    // incompletely when a session is started from an existing session tab.
    // Unfortunately, calling repaint on the console as well as the MainWindow
    // didn't fix the problem.
    static bool first = true;

    if (first == true) {
        if (this->parent() != nullptr) {
            this->setHidden(true);
            this->setHidden(false);
        }
        first = false;
    }
#endif

    if (connEntry == nullptr) {
        qDebug() << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__;
        return;
    }

    if (connEntry->password.isEmpty()) {
        disconnect(this, &SSHTermWidget::receivedData, this, &SSHTermWidget::dataReceived);
        return;
    }

    // Ignore single characters because they are user input sent back because of
    // echo mode.
    if (text.length() == 1) {
        return;
    }

    this->passwordLineCounter++;

    if (this->passwordLineCounter > 10) {
        disconnect(this, &SSHTermWidget::receivedData, this, &SSHTermWidget::dataReceived);
    }

    if (this->passwordRegex1.exactMatch(text) || this->passwordRegex2.exactMatch(text)) {
        qDebug() << "Sending ssh password...";
        this->sendText(connEntry->password + "\n");

        disconnect(this, &SSHTermWidget::receivedData, this, &SSHTermWidget::dataReceived);
    }
}

void SSHTermWidget::updateConsoleSettings(const QFont &font, const QString &colorScheme)
{
    this->setTerminalFont(font);
    this->setColorScheme(colorScheme);
    this->update();
}
