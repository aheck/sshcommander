#include "NotesEditor.h"

NotesEditor::NotesEditor()
{
    QVBoxLayout *layout = new QVBoxLayout();

    this->editor = new QTextEdit();
    this->widgetStack = new QStackedWidget();
    QWidget *editorWidget = new QWidget();

    QToolBar *toolBar = new QToolBar();
    this->actionTextBold = toolBar->addAction("", this, SLOT(formatTextBold()));
    this->actionTextBold->setCheckable(true);
    this->actionTextBold->setToolTip(tr("Bold"));
    QFont fontBold = QFont("Arial");
    fontBold.setBold(true);
    QPixmap boldIcon = this->generateCharIcon('B', fontBold);
    this->actionTextBold->setIcon(boldIcon);

    this->actionTextItalic = toolBar->addAction("", this, SLOT(formatTextItalic()));
    this->actionTextItalic->setCheckable(true);
    this->actionTextItalic->setToolTip(tr("Italic"));
    QFont fontItalic = QFont("Arial");
    fontItalic.setItalic(true);
    QPixmap italicIcon = this->generateCharIcon('I', fontItalic);
    this->actionTextItalic->setIcon(italicIcon);

    this->actionTextUnderline = toolBar->addAction("", this, SLOT(formatTextUnderline()));
    this->actionTextUnderline->setCheckable(true);
    this->actionTextUnderline->setToolTip(tr("Underline"));
    QFont fontUnderline = QFont("Arial");
    fontUnderline.setUnderline(true);
    QPixmap underlineIcon = this->generateCharIcon('U', fontUnderline);
    this->actionTextUnderline->setIcon(underlineIcon);

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

QPixmap NotesEditor::generateCharIcon(char character, QFont font)
{
    QPixmap icon(256, 256);
    icon.fill(Qt::transparent);
    QPainter painter(&icon);

    //painter.fillRect(0, 0, 256, 256, Qt::white);
    painter.setPen(Qt::black);
    font.setPointSize(156);
    painter.setFont(font);
    painter.drawText(QPoint(64, 180), QString(character));

    return icon;
}
