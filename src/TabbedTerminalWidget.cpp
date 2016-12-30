#include "TabbedTerminalWidget.h"

TabbedTerminalWidget::TabbedTerminalWidget(Preferences *preferences, std::weak_ptr<SSHConnectionEntry> connEntry, QWidget *parent) :
        QTabWidget(parent)
{
    CustomTabBar* tabBar = new CustomTabBar();

    this->preferences = preferences;
    this->connEntryWeak = connEntry;
    this->setTabBar(tabBar);
    this->setTabsClosable(true);
    this->setTabPosition(QTabWidget::North);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void TabbedTerminalWidget::addTerminalSession()
{
    auto connEntry = this->connEntryWeak.lock();

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    this->addTab(console, QString::asprintf("Session %d", connEntry->nextSessionNumber++));
    this->setCurrentWidget(console);

    console->startShellProgram();

    this->setFocus();
    console->setFocus();
}

void TabbedTerminalWidget::addInactiveSession(const QString title)
{
    InactiveSessionWidget *inactiveSessionWidget = new InactiveSessionWidget();
    connect(inactiveSessionWidget, SIGNAL(createSession()), this, SLOT(restartCurrentSession()));

    this->addTab(inactiveSessionWidget, title);
}

void TabbedTerminalWidget::restartCurrentSession()
{
    QWidget *oldWidget = nullptr;
    auto connEntry = this->connEntryWeak.lock();

    if (this->count() == 0) {
        return;
    }

    int tabIndex = this->currentIndex();
    const QString tabText = this->tabText(tabIndex);

    if (this->currentWidget() != nullptr) {
        if (QString("QTermWidget") == this->currentWidget()->metaObject()->className()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Restart Session?",
                    QString("Do you really want to restart SSH session '%1' with '%2'?").arg(tabText).arg(connEntry->name),
                    QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            }
        }

        oldWidget = this->currentWidget();
    }

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    this->setUpdatesEnabled(false);
    this->removeTab(tabIndex);
    this->insertTab(tabIndex, console, tabText);
    this->setCurrentIndex(tabIndex);
    this->setUpdatesEnabled(true);

    console->startShellProgram();

    this->setFocus();
    console->setFocus();

    if (oldWidget) {
        delete oldWidget;
    }
}

QTermWidget* TabbedTerminalWidget::createNewTermWidget(const QStringList *args, bool connectReceivedData)
{
    const QString *program = new QString("/usr/bin/ssh");

    QTermWidget *console = new QTermWidget(0);

    if (connectReceivedData) {
        connect(console, SIGNAL(receivedData(QString)), this, SLOT(dataReceived(QString)));
    }

    console->setAutoClose(false);
    console->setShellProgram(*program);
    console->setArgs(*args);
    console->setTerminalFont(this->preferences->getTerminalFont());

    console->setColorScheme(this->preferences->getColorScheme());
    console->setScrollBarPosition(QTermWidget::ScrollBarRight);

    return console;
}

void TabbedTerminalWidget::dataReceived(const QString &text)
{
    QObject *sender = QObject::sender();
    if (sender->metaObject()->className() != QString("QTermWidget")) {
        return;
    }

    auto connEntry = this->connEntryWeak.lock();

    QTermWidget *console = static_cast<QTermWidget *>(sender);
    if (connEntry == nullptr) {
        return;
    }

    if (connEntry->password.isEmpty()) {
        return;
    }

    if (text.endsWith(" password: ")) {
        console->sendText(connEntry->password + "\n");
        disconnect(console, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
    }
}

void TabbedTerminalWidget::closeTab(int tabIndex)
{
    auto connEntry = this->connEntryWeak.lock();
    QTermWidget *termWidget = (QTermWidget*) this->widget(tabIndex);

    if (termWidget != nullptr) {
        QMessageBox::StandardButton reply;
        const QString usernameAndHost = connEntry->name;
        reply = QMessageBox::question(this, "Closing Session",
                QString("Do you really want to close SSH session '%1' with '%2'?").arg(this->tabText(tabIndex)).arg(usernameAndHost),
                QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        this->removeTab(tabIndex);
        delete termWidget;
    }
}
