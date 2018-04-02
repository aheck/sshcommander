#include "TerminalViewWidget.h"

TerminalViewWidget::TerminalViewWidget(QWidget *parent) :
    QWidget(parent)
{
    this->enlarged = false;
    this->appletsShown = true;

    this->widgetStack = new QStackedWidget();

    DisabledWidget *disabledPage = new DisabledWidget("No SSH Connection");
    this->widgetStack->addWidget(disabledPage);

    this->toolBar = new QToolBar(this);
    this->toolBar->addAction(QIcon(":/images/utilities-terminal.svg"), "New Session", this, SLOT(createNewSession()));
    this->toolBar->addAction(QIcon(":/images/view-refresh.svg"), "Restart Session", this, SLOT(restartCurrentSession()));
    this->toggleWindowButton = this->toolBar->addAction(QIcon(":/images/window-new.svg"),
            "Detach Terminal");
    connect(this->toggleWindowButton, SIGNAL(toggled(bool)), this, SLOT(toggleWindowMode(bool)));
    this->toggleWindowButton->setCheckable(true);
    this->toggleEnlarged = this->toolBar->addAction(QIcon(":/images/view-fullscreen.svg"),
            "Toggle Enlarged View (F10)", this, SLOT(toggleEnlarge()));
    this->toggleEnlarged->setCheckable(true);

    this->terminalStack = new QStackedWidget();
    this->appletStack = new QStackedWidget();

    this->terminalSplitter = new QSplitter(Qt::Vertical);
    this->terminalSplitter->setObjectName("terminalSplitter");
    connect(this->terminalSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(terminalSplitterMoved(int, int)));

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
    this->terminalSplitter->setCollapsible(0, false);
    this->terminalSplitter->setStretchFactor(0, 15);
    this->terminalSplitter->setStretchFactor(1, 1);

    QSplitterHandle *handle = this->terminalSplitter->handle(1);
    QHBoxLayout *splitterHandleLayout = new QHBoxLayout();
    splitterHandleLayout->setContentsMargins(5, 0, 0, 0);
    handle->setLayout(splitterHandleLayout);
    QLabel *handleLabel = new QLabel("Applets");
    handleLabel->setStyleSheet("QLabel { color : white; }");
    this->handleButton = new QToolButton(handle);
    this->handleButton->setArrowType(Qt::DownArrow);
    connect(handleButton, SIGNAL(clicked()), this, SLOT(toggleApplets()));
    handle->layout()->addWidget(handleLabel);
    handle->layout()->addItem(new QSpacerItem(0, 80, QSizePolicy::Expanding, QSizePolicy::Expanding));
    handle->layout()->addWidget(handleButton);
    handleButton->setCursor(Qt::ArrowCursor);

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
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(updateTab()));
    connect(tabs, SIGNAL(terminalAttachmentChanged()), this, SLOT(updateTab()));
    this->terminalStack->addWidget(tabs);

    AppletWidget *applets = new AppletWidget(connEntry);
    this->appletStack->addWidget(applets);
    updateTab();
}

void TerminalViewWidget::removeConnection(TabbedTerminalWidget *tabbedTerminal)
{
    int index = this->terminalStack->indexOf(tabbedTerminal);

    tabbedTerminal->closeAllDetachedWindows();

    this->terminalStack->removeWidget(tabbedTerminal);
    this->appletStack->removeWidget(this->appletStack->widget(index));
    updateTab();
}

void TerminalViewWidget::moveConnection(int originRow, int targetRow)
{
    QWidget *tabbedTerminal = this->terminalStack->widget(originRow);
    QWidget *applet = this->appletStack->widget(originRow);

    this->terminalStack->removeWidget(tabbedTerminal);
    this->appletStack->removeWidget(applet);

    this->terminalStack->insertWidget(targetRow, tabbedTerminal);
    this->appletStack->insertWidget(targetRow, applet);

    this->setCurrentConnection(targetRow);
}

void TerminalViewWidget::setCurrentConnection(int row)
{
    this->terminalStack->setCurrentIndex(row);
    QWidget *termWidget = this->terminalStack->currentWidget();
    if (termWidget != nullptr) {
        termWidget->setFocus();
    }
    this->appletStack->setCurrentIndex(row);

    // Send the currently shown Applet an onShow signal
    AppletWidget *currentAppletWidget = static_cast<AppletWidget*>(this->appletStack->currentWidget());
    if (currentAppletWidget == nullptr) {
        return;
    }

    currentAppletWidget->appletChanged(-1);
    updateTab();
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

    if (widget == nullptr) {
        return;
    }

    auto *container = static_cast<TerminalContainer *>(widget);
    widget = container->getWidget();

    if (widget == nullptr) {
        return;
    }

    widget->setFocus();
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

void TerminalViewWidget::updateTab()
{
    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());
    TerminalContainer *container = static_cast<TerminalContainer*>(tabs->widget(tabs->currentIndex()));
    this->toggleWindowButton->setChecked(container->isDetached());

    if (container->isDetached()) {
        this->toggleWindowButton->setToolTip(tr("Reattach Terminal"));
    } else {
        this->toggleWindowButton->setToolTip(tr("Detach Terminal"));
    }
}

void TerminalViewWidget::toggleWindowMode(bool checked)
{
    if (this->terminalStack->count() == 0) {
        return;
    }

    TabbedTerminalWidget *tabs = static_cast<TabbedTerminalWidget *>(this->terminalStack->currentWidget());

    if (checked) {
        tabs->detachTab(tabs->currentIndex());
    } else {
        TerminalContainer *container = static_cast<TerminalContainer*>(tabs->widget(tabs->currentIndex()));
        tabs->reattachTab(container->getUuid());
    }
}

void TerminalViewWidget::setAppletsShown(bool appletsShown)
{
    this->appletsShown = appletsShown;

    if (appletsShown) {
        this->handleButton->setArrowType(Qt::DownArrow);
    } else {
        this->handleButton->setArrowType(Qt::UpArrow);
    }
}

void TerminalViewWidget::toggleApplets()
{
    QList<int> sizes = this->terminalSplitter->sizes();

    if (this->appletsShown) {
        this->terminalSize = sizes.at(0);
        this->appletSize = sizes.at(1);
        sizes.replace(1, 0);
        this->terminalSplitter->setSizes(sizes);

        this->setAppletsShown(false);
    } else {
        sizes.replace(0, this->terminalSize);
        sizes.replace(1, this->appletSize);
        this->terminalSplitter->setSizes(sizes);

        this->setAppletsShown(true);
    }
}

void TerminalViewWidget::terminalSplitterMoved(int pos, int index)
{
    QList<int> sizes = this->terminalSplitter->sizes();

    if (sizes.at(1) == 0) {
        this->setAppletsShown(false);
    } else {
        this->setAppletsShown(true);
    }
}

void TerminalViewWidget::showEvent(QShowEvent *event)
{
    QList<int> sizes = this->terminalSplitter->sizes();
    this->terminalSize = sizes.at(0);
    this->appletSize = sizes.at(1);
}
