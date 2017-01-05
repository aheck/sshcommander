#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog()
{
    this->setWindowTitle(tr("Preferences"));
    this->setWindowIcon(QIcon(":/images/preferences-system.svg"));
    QVBoxLayout *layout = new QVBoxLayout();

    this->tabs = new QTabWidget();

    // Terminal tab
    QWidget *terminalPage = new QWidget();
    QFormLayout *terminalLayout = new QFormLayout();

    this->fontButton = new QPushButton("");
    QObject::connect(this->fontButton, SIGNAL (clicked()), this, SLOT (selectFont()));
    terminalLayout->addRow(tr("Font:"), fontButton);

    this->colorSchemeComboBox = new QComboBox();
    this->colorSchemeComboBox->addItems(QTermWidget::availableColorSchemes());
    terminalLayout->addRow(tr("Color Scheme:"), this->colorSchemeComboBox);

    terminalPage->setLayout(terminalLayout);
    this->tabs->addTab(terminalPage, tr("Terminal"));

    // AWS tab
    QWidget *awsPage = new QWidget();
    QFormLayout *awsLayout = new QFormLayout();

    this->accessKeyLineEdit = new QLineEdit();
    awsLayout->addRow(tr("Access Key:"), this->accessKeyLineEdit);

    this->secretKeyLineEdit = new QLineEdit();
    awsLayout->addRow(tr("Secret Key:"), this->secretKeyLineEdit);

    awsPage->setLayout(awsLayout);
    this->tabs->addTab(awsPage, tr("AWS"));

    // General dialog layout
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

const QString PreferencesDialog::getAWSAccessKey()
{
    return this->accessKeyLineEdit->text();
}

void PreferencesDialog::setAWSAccessKey(const QString accessKey)
{
    this->accessKeyLineEdit->setText(accessKey);
}

const QString PreferencesDialog::getAWSSecretKey()
{
    return this->secretKeyLineEdit->text();
}

void PreferencesDialog::setAWSSecretKey(const QString secretKey)
{
    this->secretKeyLineEdit->setText(secretKey);
}

const QString PreferencesDialog::getColorScheme()
{
    return this->colorSchemeComboBox->currentText();
}

void PreferencesDialog::setColorScheme(const QString colorScheme)
{
    this->colorSchemeComboBox->setCurrentText(colorScheme);
}
