#include "TerminalViewWidget.h"

TerminalViewWidget::TerminalViewWidget(QWidget *parent) :
    QWidget(parent)
{
    this->enlarged = false;

    this->widgetStack = new QStackedWidget();

    DisabledWidget *disabledPage = new DisabledWidget("No SSH Connection");
    this->widgetStack->addWidget(disabledPage);

    this->toolBar = new QToolBar(this);
    this->toolBar->addAction(QIcon(":/images/utilities-terminal.svg"), "New Session", this, SLOT(createNewSession()));
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Restart Session", this, SLOT(restartCurrentSession()));
    this->toggleEnlarged = this->toolBar->addAction(QIcon(":/images/view-fullscreen.svg"),
            "Toggle Enlarged View (F10)", this, SLOT(toggleEnlarge()));
    this->toggleEnlarged->setCheckable(true);

    this->terminalStack = new QStackedWidget();
    this->appletStack = new QStackedWidget();

    this->terminalSplitter = new QSplitter(Qt::Vertical);

    this->terminalSplitter->setCollapsible(0, false);

    // build layout
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->setSpacing(0);
    boxLayout->setMargin(0);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->addWidget(this->toolBar);
    boxLayout->addWidget(this->terminalStack);

    this->terminalPage = new QWidget();
    this->terminalPage->setLayout(boxLayout);

    this->terminalSplitter->addWidget(this->terminalPage);
    this->terminalSplitter->addWidget(this->appletStack);

    this->widgetStack->addWidget(this->terminalSplitter);

    // assign shortcuts
    QShortcut *toggleEnlargedShortcut = new QShortcut(QKeySequence(Qt::Key_F10), this);
    toggleEnlargedShortcut->setContext(Qt::ApplicationShortcut);
    connect(toggleEnlargedShortcut, SIGNAL(activated()), this, SLOT(toggleEnlarge()));

    QShortcut *nextTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this);
    nextTabShortcut->setContext(Qt::ApplicationShortcut);
    connect(nextTabShortcut, SIGNAL(activated()), this, SLOT(nextTab()));

    QShortcut *prevTabShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this);
    prevTabShortcut->setContext(Qt::ApplicationShortcut);
    connect(prevTabShortcut, SIGNAL(activated()), this, SLOT(prevTab()));

    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->widgetStack);
}

void TerminalViewWidget::addConnection(std::shared_ptr<SSHConnectionEntry> connEntry, TabbedTerminalWidget *tabs)
{
    this->terminalStack->addWidget(tabs);

    AppletWidget *applets = new AppletWidget(connEntry);
    this->appletStack->addWidget(applets);
}

void TerminalViewWidget::removeConnection(TabbedTerminalWidget *tabbedTerminal)
{
    int index = this->terminalStack->indexOf(tabbedTerminal);

    tabbedTerminal->closeAllDetachedWindows();

    this->terminalStack->removeWidget(tabbedTerminal);
    this->appletStack->removeWidget(this->appletStack->widget(index));
}

void TerminalViewWidget::setCurrentConnection(int row)
{
    this->terminalStack->setCurrentIndex(row);
    this->appletStack->setCurrentIndex(row);

    // Send the currently shown Applet a onShow signal
    AppletWidget *currentAppletWidget = static_cast<AppletWidget*>(this->appletStack->currentWidget());
    currentAppletWidget->appletChanged(-1);
}

void TerminalViewWidget::setLastConnection()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    this->setCurrentConnection(this->terminalStack->count() - 1);
}

void TerminalViewWidget::setDisabledPageEnabled(bool enabled)
{
    if (enabled) {
        this->widgetStack->setCurrentIndex(0);
    } else {
        this->widgetStack->setCurrentIndex(1);
    }
}

void TerminalViewWidget::createNewSession()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());
    tabs->addTerminalSession();
}

void TerminalViewWidget::restartCurrentSession()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());
    tabs->restartCurrentSession();
}

void TerminalViewWidget::setFocusOnCurrentTerminal()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    QWidget *widget = this->terminalStack->currentWidget();
    auto *tabs = static_cast<TabbedTerminalWidget *>(widget);
    widget = tabs->currentWidget();

    if (widget != nullptr) {
        widget->setFocus();
    }
}

void TerminalViewWidget::nextTab()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());

    if (tabs->currentIndex() < (tabs->count() - 1)) {
        tabs->setCurrentIndex(tabs->currentIndex() + 1);
    } else {
        tabs->setCurrentIndex(0);
    }

    this->setFocusOnCurrentTerminal();
}

void TerminalViewWidget::prevTab()
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());

    if (tabs->currentIndex() > 0) {
        tabs->setCurrentIndex(tabs->currentIndex() - 1);
    } else {
        tabs->setCurrentIndex(tabs->count() - 1);
    }

    this->setFocusOnCurrentTerminal();
}

void TerminalViewWidget::toggleEnlarge()
{
    emit requestToggleEnlarge();

    this->enlarged = !this->enlarged;
    this->toggleEnlarged->setChecked(this->enlarged);
    this->setFocusOnCurrentTerminal();
}

void TerminalViewWidget::updateConsoleSettings(const QFont &font, const QString colorScheme)
{
    // update all running QTermWidget instances
    for (int i = 0; i < this->terminalStack->count(); i++) {
        TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->widget(i));

        for (int j = 0; j < tabs->count(); j++) {
            QWidget *widget = tabs->widget(j);
            if (widget->metaObject()->className() == QString("QTermWidget")) {
                QTermWidget *console = (QTermWidget*) widget;
                console->setTerminalFont(font);
                console->setColorScheme(colorScheme);
                console->update();
            }
        }
    }
}
