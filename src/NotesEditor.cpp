#include "NotesEditor.h"

NotesEditor::NotesEditor()
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->editor = new QTextEdit();
    this->widgetStack = new QStackedWidget();
    QWidget *editorWidget = new QWidget();

    QToolBar *toolBar = new QToolBar();

    this->comboFont = new QFontComboBox();
    toolBar->addWidget(this->comboFont);
    connect(this->comboFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(changeFont(QFont)));

    this->comboSize = new QComboBox();
    this->comboSize->setObjectName("comboSize");
    toolBar->addWidget(this->comboSize);
    this->comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        comboSize->addItem(QString::number(size));
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
    layout->addWidget(toolBar);

    layout->addWidget(this->editor);
    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(this->editor, SIGNAL(textChanged()),
            this, SLOT(textWasChanged()));
    connect(this->editor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    editorWidget->setLayout(layout);

    this->disabledWidget = new DisabledWidget("No SSH Connection");

    this->widgetStack->addWidget(this->disabledWidget);
    this->widgetStack->addWidget(editorWidget);
    this->widgetStack->setCurrentIndex(0);

    this->setLayout(new QVBoxLayout());
    this->layout()->addWidget(widgetStack);
}

void NotesEditor::setEnabled(bool enabled)
{
    if (enabled) {
        this->widgetStack->setCurrentIndex(1);
    } else {
        this->widgetStack->setCurrentIndex(0);
    }
}

void NotesEditor::setHtml(const QString &text)
{
    this->editor->setHtml(text);
}

QString NotesEditor::toHtml() const
{
    return this->editor->toHtml();
}

void NotesEditor::textWasChanged()
{
    emit textChanged();
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
    painter.setPen(Qt::black);
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
    painter.setPen(Qt::black);
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