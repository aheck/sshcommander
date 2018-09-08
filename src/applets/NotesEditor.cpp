#include "NotesEditor.h"

#include "MainWindow.h"

NotesEditor::NotesEditor()
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->editor = new QTextEdit();
    this->editor->setTextColor(Qt::black);
    this->editor->setTextBackgroundColor(Qt::white);
    QWidget *editorWidget = new QWidget();

    QToolBar *toolBar = new QToolBar();

    this->actionSave = toolBar->addAction("", this, SLOT(save()));
    this->actionSave->setIcon(QIcon(":/images/document-save.svg"));
    this->actionSave->setToolTip("Save");

    toolBar->addSeparator();

    this->comboFont = new QFontComboBox();
    toolBar->addWidget(this->comboFont);
    connect(this->comboFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(changeFont(QFont)));

    this->comboSize = new QComboBox();
    this->comboSize->setObjectName("comboSize");
    toolBar->addWidget(this->comboSize);
    this->comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes) {
        comboSize->addItem(QString::number(size));
    }
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, SIGNAL(currentTextChanged(QString)), this, SLOT(changeFontSize(QString)));

    toolBar->addSeparator();

    actionUndo = toolBar->addAction(QIcon(":/images/edit-undo.svg"), tr("Undo"), this->editor, SLOT(undo()));
    actionUndo->setShortcut(QKeySequence::Undo);

    actionRedo = toolBar->addAction(QIcon(":/images/edit-redo.svg"), tr("Redo"), this->editor, SLOT(redo()));
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);

    toolBar->addSeparator();

    this->actionTextBold = toolBar->addAction("", this, SLOT(formatTextBold()));
    this->actionTextBold->setIcon(QIcon(":/images/format-text-bold.svg"));
    this->actionTextBold->setCheckable(true);
    this->actionTextBold->setToolTip(tr("Bold"));

    this->actionTextItalic = toolBar->addAction("", this, SLOT(formatTextItalic()));
    this->actionTextItalic->setIcon(QIcon(":/images/format-text-italic.svg"));
    this->actionTextItalic->setCheckable(true);
    this->actionTextItalic->setToolTip(tr("Italic"));

    this->actionTextUnderline = toolBar->addAction("", this, SLOT(formatTextUnderline()));
    this->actionTextUnderline->setIcon(QIcon(":/images/format-text-underline.svg"));
    this->actionTextUnderline->setCheckable(true);
    this->actionTextUnderline->setToolTip(tr("Underline"));

    toolBar->addSeparator();

    this->actionTextColor = toolBar->addAction(QIcon(), tr("Text Color"), this,
            SLOT(selectTextColor()));
    colorChanged(Qt::black);
    this->actionTextBackgroundColor = toolBar->addAction(QIcon(), tr("Background Color"), this,
            SLOT(selectBackgroundColor()));
    backgroundColorChanged(Qt::white);

    this->statusBar = new QStatusBar();

    layout->addWidget(toolBar);

    layout->addWidget(this->editor);
    layout->addWidget(this->statusBar);
    this->statusBar->setHidden(true);
    connect(this->statusBar, SIGNAL(messageChanged(QString)), this, SLOT(statusBarChanged(QString)));

    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(this->editor, SIGNAL(textChanged()),
            this, SLOT(textWasChanged()));
    connect(this->editor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    editorWidget->setLayout(layout);

    this->setLayout(new QVBoxLayout());
    this->layout()->setSpacing(0);
    this->layout()->setMargin(0);
    this->layout()->setContentsMargins(0, 0, 0, 0);

    this->layout()->addWidget(editorWidget);
}

const QString NotesEditor::getDisplayName()
{
    return tr("Notes");
}

QIcon NotesEditor::getIcon()
{
    return QIcon(":/images/accessories-text-editor.svg");
}

void NotesEditor::init(std::shared_ptr<SSHConnectionEntry> connEntry)
{
    Applet::init(connEntry);

    this->setHtml(connEntry->notes);
}

void NotesEditor::setHtml(const QString &text)
{
    // QTextEditor forgets to save background color white with empty paragraphs
    // which makes them become black (although the background is still displayed white)
    //
    // By adding the background color where it is missing we prevent the
    // background color button from showing the wrong color.
    QString processedText = text;
    QRegularExpression regex("(\"-qt-paragraph-type:empty;((?!background-color)[^\"])*\")");

    QRegularExpressionMatch match = regex.match(processedText);
    while (match.hasMatch()) {
        QString oldString = match.captured(1);
        QString newString = oldString;

        newString.chop(1); // remove the "
        newString += " background-color: #ffffff;\"";

        processedText.replace(oldString, newString);
        match = regex.match(processedText);
    }

    this->editor->setHtml(processedText);
}

QString NotesEditor::toHtml() const
{
    return this->editor->toHtml();
}

void NotesEditor::textWasChanged()
{
    if (connEntry == nullptr) {
        return;
    }

    this->connEntry->notes = this->toHtml();
}

void NotesEditor::changeFont(const QFont &font)
{
    QTextCharFormat format;

    format.setFontFamily(font.family());
    applyFormat(format);
}

void NotesEditor::changeFontSize(const QString fontSize)
{
    bool ok = false;
    int size = fontSize.toInt(&ok, 10);

    if (!ok) {
        return;
    }

    QTextCharFormat format;

    format.setFontPointSize(size);
    applyFormat(format);
}

void NotesEditor::formatTextBold()
{
    QTextCharFormat format;

    format.setFontWeight(this->actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    applyFormat(format);
}

void NotesEditor::formatTextItalic()
{
    QTextCharFormat format;

    format.setFontItalic(this->actionTextItalic->isChecked());
    applyFormat(format);
}

void NotesEditor::formatTextUnderline()
{
    QTextCharFormat format;

    format.setFontUnderline(this->actionTextUnderline->isChecked());
    applyFormat(format);
}

void NotesEditor::selectTextColor()
{
    QColor color = QColorDialog::getColor(this->editor->textColor(), this);

    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    this->applyFormat(format);
    colorChanged(color);
}

void NotesEditor::selectBackgroundColor()
{
    QColor color = QColorDialog::getColor(this->editor->textBackgroundColor(), this);

    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setBackground(color);
    this->applyFormat(format);
    backgroundColorChanged(color);
}

void NotesEditor::applyFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = editor->textCursor();

    cursor.mergeCharFormat(format);
    editor->mergeCurrentCharFormat(format);
}

void NotesEditor::colorChanged(const QColor &color)
{
    if (!color.isValid()) {
        return;
    }

    QPixmap pic(16, 16);

    pic.fill(color);
    QPainter painter(&pic);

    // determine if we paint a black or white border
    if (color.lightness() < 128) {
        painter.setPen(Qt::white);
    } else {
        painter.setPen(Qt::black);
    }

    painter.drawRect(0, 0, 15, 15);
    this->actionTextColor->setIcon(pic);
}

void NotesEditor::backgroundColorChanged(const QColor &color)
{
    if (!color.isValid()) {
        return;
    }

    QPixmap pic(16, 16);

    pic.fill(color);
    QPainter painter(&pic);

    // determine if we paint a black or white border
    if (color.lightness() < 128) {
        painter.setPen(Qt::white);
    } else {
        painter.setPen(Qt::black);
    }

    painter.drawRect(0, 0, 15, 15);
    this->actionTextBackgroundColor->setIcon(pic);
}

void NotesEditor::currentCharFormatChanged(const QTextCharFormat &format)
{
    this->fontChanged(format.font());
    this->colorChanged(format.foreground().color());
    this->backgroundColorChanged(format.background().color());
}

void NotesEditor::fontChanged(const QFont &font)
{
    QString fontSize = QString::number(font.pointSize());

    this->comboFont->setCurrentFont(font);
    this->comboSize->setCurrentText(fontSize);
    this->actionTextBold->setChecked(font.bold());
    this->actionTextItalic->setChecked(font.italic());
    this->actionTextUnderline->setChecked(font.underline());
}

void NotesEditor::save()
{
    MainWindow *mainWindow = Util::getMainWindow();

    if (mainWindow == nullptr) {
        this->statusBar->showMessage(tr("Failed to save"), 2000);
        return;
    }

    mainWindow->saveSettings();
    this->statusBar->showMessage(tr("Saved"), 2000);
}

void NotesEditor::statusBarChanged(QString message)
{
    if (message.isEmpty()) {
        this->statusBar->setHidden(true);
    } else {
        this->statusBar->setHidden(false);
    }
}
