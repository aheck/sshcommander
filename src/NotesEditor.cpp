#include "NotesEditor.h"

NotesEditor::NotesEditor()
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->editor = new QTextEdit();
    this->widgetStack = new QStackedWidget();
    QWidget *editorWidget = new QWidget();

    QToolBar *toolBar = new QToolBar();
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

    QPixmap pic(16, 16);
    pic.fill(Qt::black);
    this->actionTextColor = toolBar->addAction(pic, tr("Text Color"), this,
            SLOT(selectTextColor()));
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

void NotesEditor::applyFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = editor->textCursor();

    cursor.mergeCharFormat(format);
    editor->mergeCurrentCharFormat(format);
}

void NotesEditor::colorChanged(const QColor &color)
{
    QPixmap pic(16, 16);

    pic.fill(color);
    this->actionTextColor->setIcon(pic);
}

void NotesEditor::currentCharFormatChanged(const QTextCharFormat &format)
{
    this->fontChanged(format.font());
    this->colorChanged(format.foreground().color());
}

void NotesEditor::fontChanged(const QFont &font)
{
    this->actionTextBold->setChecked(font.bold());
    this->actionTextItalic->setChecked(font.italic());
    this->actionTextUnderline->setChecked(font.underline());
}
