#include "TabbedTerminalWidget.h"

TerminalSessionEntry::TerminalSessionEntry()
{
    this->uuid = QUuid::createUuid();
    this->detached = false;
    this->container = nullptr;
    this->window = nullptr;
}

TabbedTerminalWidget::TabbedTerminalWidget(std::weak_ptr<SSHConnectionEntry> connEntry, QWidget *parent) :
        QTabWidget(parent)
{
    CustomTabBar* tabBar = new CustomTabBar();

    this->connEntryWeak = connEntry;
    this->setTabBar(tabBar);
    this->setTabsClosable(true);
    this->setTabPosition(QTabWidget::North);
    this->setMovable(true);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabBar, SIGNAL(tabDetachRequested(int)), this, SLOT(detachTab(int)), Qt::QueuedConnection);
}

TabbedTerminalWidget::~TabbedTerminalWidget()
{
    for (int i = this->count(); i >= 0; --i) {
        QTermWidget *termWidget = (QTermWidget*) this->widget(i);
        this->removeTab(i);
        delete termWidget;
    }
}

void TabbedTerminalWidget::addTerminalSession()
{
    auto connEntry = this->connEntryWeak.lock();

    TerminalSessionEntry *terminalSession = new TerminalSessionEntry();

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());
    TerminalContainer *container = new TerminalContainer(terminalSession->uuid);
    container->setWidget(console);
    this->addTab(container, "Session " + QString::number(connEntry->nextSessionNumber++));
    this->setCurrentWidget(container);

    terminalSession->container = container;
    this->terminalSessions[terminalSession->uuid] = terminalSession;

    console->startShellProgram();

    console->setFocus();
}

void TabbedTerminalWidget::addInactiveSession(const QString title)
{
    TerminalSessionEntry *terminalSession = new TerminalSessionEntry();
    InactiveSessionWidget *inactiveSessionWidget = new InactiveSessionWidget(terminalSession->uuid);

    TerminalContainer *container = new TerminalContainer(terminalSession->uuid);
    container->setWidget(inactiveSessionWidget);
    connect(inactiveSessionWidget, SIGNAL(createSession(QUuid)), this, SLOT(startInactiveSession(QUuid)));

    terminalSession->container = container;
    this->terminalSessions[terminalSession->uuid] = terminalSession;

    this->addTab(container, title);
}

void TabbedTerminalWidget::closeAllDetachedWindows()
{
    for (auto iter : this->terminalSessions) {
        if (iter.second->detached) {
            iter.second->window->close();
        }
    }
}

void TabbedTerminalWidget::startInactiveSession(QUuid uuid)
{
    std::cout << "startInactiveSession\n";

    QWidget *oldWidget = nullptr;
    auto connEntry = this->connEntryWeak.lock();
    TerminalSessionEntry *terminalEntry = this->terminalSessions[uuid];

    QString sessionName = terminalEntry->sessionName;
    TerminalContainer *container = terminalEntry->container;

    if (container->getWidgetClassname() == "QTermWidget") {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Restart Session?",
                QString("Do you really want to restart SSH session '%1' with '%2'?").arg(sessionName).arg(connEntry->name),
                QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    QStringList args = connEntry->generateCliArgs();
    QTermWidget *console = createNewTermWidget(&args, !connEntry->password.isEmpty());

    if (!terminalEntry->detached) {
        oldWidget = container->getWidget();

        if (oldWidget != nullptr) {
            container->getWidget()->deleteLater();
        }

        container->setWidget(console);
    } else {
        terminalEntry->window->layout()->takeAt(0)->widget()->deleteLater();
        terminalEntry->window->layout()->addWidget(console);
    }

    console->startShellProgram();

    console->setFocus();
}

void TabbedTerminalWidget::restartCurrentSession()
{
    if (this->count() == 0) {
        return;
    }

    TerminalContainer *container = static_cast<TerminalContainer*>(this->currentWidget());

    startInactiveSession(container->getUuid());
}

QTermWidget* TabbedTerminalWidget::createNewTermWidget(const QStringList *args, bool connectReceivedData)
{
    const QString *program = new QString("/usr/bin/ssh");

    QTermWidget *console = new QTermWidget(0);

    if (connectReceivedData) {
        connect(console, SIGNAL(receivedData(QString)), this, SLOT(dataReceived(QString)));
    }

    Preferences &preferences = Preferences::getInstance();

    console->setAutoClose(false);
    console->setShellProgram(*program);
    console->setArgs(*args);
    console->setTerminalFont(preferences.getTerminalFont());

    console->setColorScheme(preferences.getColorScheme());
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

    if (!connEntry) {
        std::cerr << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__ << std::endl;
        return;
    }

    QTermWidget *console = static_cast<QTermWidget *>(sender);
    if (connEntry == nullptr) {
        return;
    }

    if (connEntry->password.isEmpty()) {
        return;
    }

    if (this->passwordLineCounter.count(console) == 0) {
        this->passwordLineCounter[console] = 1;
    } else {
        this->passwordLineCounter[console]++;

        if (this->passwordLineCounter[console] > 10) {
            disconnect(console, SIGNAL(receivedData(QString)),
                    this, SLOT(dataReceived(QString)));
            this->passwordLineCounter.erase(console);
        }
    }

    if (QRegExp("^.*( )?(p|P)assword:( )?$").exactMatch(text) || QRegExp("Password for \\S+@\\S+:").exactMatch(text)) {
        console->sendText(connEntry->password + "\n");

        disconnect(console, SIGNAL(receivedData(QString)),
                this, SLOT(dataReceived(QString)));
        this->passwordLineCounter.erase(console);
    }
}

void TabbedTerminalWidget::closeTab(int tabIndex)
{
    auto connEntry = this->connEntryWeak.lock();

    if (!connEntry) {
        std::cerr << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__ << std::endl;
        return;
    }

    TerminalContainer *container = static_cast<TerminalContainer*>(this->widget(tabIndex));

    if (container != nullptr) {
        QMessageBox::StandardButton reply;
        const QString usernameAndHost = connEntry->name;
        reply = QMessageBox::question(this, "Closing Session",
                QString("Do you really want to close SSH session '%1' with '%2'?").arg(this->tabText(tabIndex)).arg(usernameAndHost),
                QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }

        QUuid uuid(container->getUuid());
        TerminalSessionEntry *terminalEntry = this->terminalSessions[uuid];
        this->terminalSessions.erase(uuid);
        delete terminalEntry;

        this->removeTab(tabIndex);
        delete container;
    }
}

void TabbedTerminalWidget::detachTab(int index)
{
    auto connEntry = this->connEntryWeak.lock();

    if (!connEntry) {
        std::cerr << "Failed to acquire shared_ptr on connEntryWeak in " <<
            __FILE__ << ":" << __LINE__ << std::endl;
        return;
    }

    DetachedTerminalWindow *window = new DetachedTerminalWindow(this);
    window->setWindowTitle(this->tabText(index) + " - " + connEntry->name);
    connect(window, SIGNAL(tabReattachRequested(QUuid)), this, SLOT(reattachTab(QUuid)));

    TerminalContainer *container = static_cast<TerminalContainer*>(this->widget(index));

    DetachedSessionWidget *newWidget = new DetachedSessionWidget();
    newWidget->setUuid(container->getUuid());
    connect(newWidget, SIGNAL(requestShowWindow(QUuid)), this, SLOT(showDetachedWindow(QUuid)));

    TerminalSessionEntry *terminalEntry = this->terminalSessions[container->getUuid()];

    QWidget *termWidget = container->getWidget();
    container->setWidget(newWidget);

    termWidget->setParent(window);
    window->setUuid(container->getUuid());
    window->layout()->addWidget(termWidget);

    terminalEntry->detached = true;
    terminalEntry->window = window;

    termWidget->show();
    window->show();
}

void TabbedTerminalWidget::reattachTab(QUuid uuid)
{
    TerminalSessionEntry *terminalEntry = this->terminalSessions[uuid];

    if (terminalEntry->detached == false) {
        return;
    }

    QWidget *window = terminalEntry->window;
    QWidget *termWidget = window->layout()->itemAt(0)->widget();

    window->layout()->removeWidget(termWidget);
    termWidget->setParent(this);

    terminalEntry->container->getWidget()->deleteLater();
    terminalEntry->container->setWidget(termWidget);
    window->hide();

    terminalEntry->detached = false;
    terminalEntry->window = nullptr;

    termWidget->setFocus();

    window->deleteLater();
}

void TabbedTerminalWidget::showDetachedWindow(QUuid uuid)
{
    TerminalSessionEntry *terminalEntry = this->terminalSessions[uuid];

    terminalEntry->window->raise();
    terminalEntry->window->activateWindow();
}
