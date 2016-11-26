#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog()
{
    this->setWindowTitle(tr("Preferences"));
    QVBoxLayout *layout = new QVBoxLayout();

    this->tabs = new QTabWidget();

    QWidget *terminalPage = new QWidget();
    QFormLayout *terminalLayout = new QFormLayout();

    this->fontButton = new QPushButton("");
    QObject::connect(this->fontButton, SIGNAL (clicked()), this, SLOT (selectFont()));
    terminalLayout->addRow(tr("Font:"), fontButton);

    this->fontColorButton = new QPushButton();
    QObject::connect(this->fontColorButton, SIGNAL (clicked()), this, SLOT (selectFontColor()));
    terminalLayout->addRow(tr("Text Color:"), this->fontColorButton);

    this->backgroundColorButton = new QPushButton();
    QObject::connect(this->backgroundColorButton, SIGNAL (clicked()), this, SLOT (selectBackgroundColor()));
    terminalLayout->addRow(tr("Background Color:"), this->backgroundColorButton);

    terminalPage->setLayout(terminalLayout);

    this->tabs->addTab(terminalPage, tr("Terminal"));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("OK"));
    QObject::connect(okButton, SIGNAL (clicked()), this, SLOT (accept()));
    buttonLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(this->tabs);
    layout->addLayout(buttonLayout);
    this->setLayout(layout);
}

void PreferencesDialog::selectFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this->terminalFont, this,
            tr("Select a terminal font..."), QFontDialog::MonospacedFonts);

    if (ok) {
        this->setFont(font);
    }
}

QFont PreferencesDialog::getFont()
{
    return this->terminalFont;
}

void PreferencesDialog::setFont(const QFont &font)
{
    this->terminalFont = font;

    this->fontButton->setText(this->terminalFont.family() + ", " + QString::number(this->terminalFont.pointSize()));
    QFont buttonFont = this->terminalFont;
    buttonFont.setPointSize(12);
    this->fontButton->setFont(buttonFont);
}

void PreferencesDialog::setFontColor(const QColor &color)
{
    this->fontColor = color;
    QString styleSheet = QString("QPushButton {background-color: rgb(%1, %2, %3);}").arg(
            this->fontColor.red()).arg(this->fontColor.green()).arg(this->fontColor.blue());
    this->fontColorButton->setStyleSheet(styleSheet);
}

void PreferencesDialog::setBackgroundColor(const QColor &color)
{
    this->backgroundColor = color;
    QString styleSheet = QString("QPushButton {background-color: rgb(%1, %2, %3);}").arg(
            this->backgroundColor.red()).arg(this->backgroundColor.green()).arg(this->backgroundColor.blue());
    this->backgroundColorButton->setStyleSheet(styleSheet);
}

void PreferencesDialog::selectFontColor()
{
    QColor color = QColorDialog::getColor(this->fontColor, this, "Select a font color...");

    if (color.isValid()) {
        this->setFontColor(color);
    }
}

void PreferencesDialog::selectBackgroundColor()
{
    QColor color = QColorDialog::getColor(this->backgroundColor, this, "Select a background color...");

    if (color.isValid()) {
        this->setBackgroundColor(color);
    }
}
