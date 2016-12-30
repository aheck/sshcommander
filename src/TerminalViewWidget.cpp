#include "TerminalViewWidget.h"

TerminalViewWidget::TerminalViewWidget(QWidget *parent) :
    QWidget(parent)
{
    this->enlarged = false;

    this->widgetStack = new QStackedWidget();

    DisabledWidget *disabledPage = new DisabledWidget("No SSH Connection");
    this->widgetStack->addWidget(disabledPage);

    toolBar = new QToolBar(this);
    toolBar->addAction(QIcon(":/images/utilities-terminal"), "New Session", this, SLOT(createNewSession()));
    toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Restart Session", this, SLOT(restartCurrentSession()));
    this->toggleEnlarged = toolBar->addAction(QIcon(":/images/view-fullscreen.svg"),
            "Toggle Enlarged View (F10)", this, SLOT(toggleEnlarge()));
    this->toggleEnlarged->setCheckable(true);

    this->connectionStack = new QStackedWidget();
    QVBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->addWidget(toolBar);
    boxLayout->addWidget(this->connectionStack);

    this->terminalPage = new QWidget();
    this->terminalPage->setLayout(boxLayout);

    this->widgetStack->addWidget(this->terminalPage);

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

void TerminalViewWidget::addConnection(TabbedTerminalWidget *tabbedTerminal)
{
    this->connectionStack->addWidget(tabbedTerminal);
}

void TerminalViewWidget::removeConnection(TabbedTerminalWidget *tabbedTerminal)
{
    this->connectionStack->removeWidget(tabbedTerminal);
}

void TerminalViewWidget::setCurrentConnection(int row)
{
    this->connectionStack->setCurrentIndex(row);
}

void TerminalViewWidget::setLastConnection()
{
    if (this->connectionStack->count() == 0) {
        return;
    }

    this->setCurrentConnection(this->connectionStack->count() - 1);
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
    if (this->connectionStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->connectionStack->currentWidget());
    tabs->addTerminalSession();
}

void TerminalViewWidget::restartCurrentSession()
{
    if (this->connectionStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->connectionStack->currentWidget());
    tabs->restartCurrentSession();
}

void TerminalViewWidget::setFocusOnCurrentTerminal()
{
    if (this->connectionStack->count() == 0) {
        return;
    }

    QWidget *widget = this->connectionStack->currentWidget();
    auto *tabs = static_cast<TabbedTerminalWidget *>(widget);
    widget = tabs->currentWidget();

    if (widget != nullptr) {
        widget->setFocus();
    }
}

void TerminalViewWidget::nextTab()
{
    if (this->connectionStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->connectionStack->currentWidget());

    if (tabs->currentIndex() < (tabs->count() - 1)) {
        tabs->setCurrentIndex(tabs->currentIndex() + 1);
    } else {
        tabs->setCurrentIndex(0);
    }

    this->setFocusOnCurrentTerminal();
}

void TerminalViewWidget::prevTab()
{
    if (this->connectionStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->connectionStack->currentWidget());

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
    for (int i = 0; i < this->connectionStack->count(); i++) {
        TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->connectionStack->widget(i));

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
