#include "PreferencesDialog.h"
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setMinimumWidth(400);
    this->setMinimumHeight(300);
    this->setWindowTitle(tr("Preferences"));
    this->setWindowIcon(QIcon(":/images/preferences-system.svg"));
    QVBoxLayout *layout = new QVBoxLayout();

    this->pageList = new PreferencesListWidget();
    this->pageList->setWrapping(false);
    this->pageList->setUniformItemSizes(true);
    this->pageList->setMovement(QListView::Static);
    this->pageList->setFlow(QListView::LeftToRight);
    this->pageList->setSelectionRectVisible(true);
    this->pageList->setIconSize(QSize(64, 64));
    this->pageList->setSelectionBehavior(QAbstractItemView::SelectItems);
    this->pageList->setSelectionMode(QAbstractItemView::SingleSelection	);
    this->pageList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(this->pageList, &PreferencesListWidget::itemSelectionChanged, this, &PreferencesDialog::pageSelectionChanged);

    QListWidgetItem *terminalItem = new QListWidgetItem("Terminal");
    terminalItem->setIcon(QIcon(":/images/utilities-terminal.svg"));
    terminalItem->setSizeHint(QSize(64, 64));
    terminalItem->setToolTip(tr("Terminal"));
    this->pageList->addItem(terminalItem);
    QListWidgetItem *awsItem = new QListWidgetItem("Amazon Web Services");
    awsItem->setIcon(QIcon(":/images/connection-type-aws.svg"));
    awsItem->setSizeHint(QSize(64, 64));
    awsItem->setToolTip(tr("Amazon Web Services"));
    this->pageList->addItem(awsItem);

    this->pages = new QStackedWidget();

    // set focus on first icon
    this->pageList->setCurrentItem(terminalItem);

    // Terminal tab
    QWidget *terminalPage = new QWidget();
    QFormLayout *terminalLayout = new QFormLayout();

    this->fontButton = new QPushButton("");
    QObject::connect(this->fontButton, &QPushButton::clicked, this, &PreferencesDialog::selectFont);
    terminalLayout->addRow(tr("Font:"), fontButton);

    this->colorSchemeComboBox = new QComboBox();
    this->colorSchemeComboBox->addItems(QTermWidget::availableColorSchemes());
    terminalLayout->addRow(tr("Color Scheme:"), this->colorSchemeComboBox);

    terminalPage->setLayout(terminalLayout);
    this->pages->addWidget(terminalPage);

    // AWS tab
    QWidget *awsPage = new QWidget();
    QFormLayout *awsLayout = new QFormLayout();

    this->accessKeyLineEdit = new QLineEdit();
    awsLayout->addRow(tr("Access Key:"), this->accessKeyLineEdit);

    this->secretKeyLineEdit = new QLineEdit();
    awsLayout->addRow(tr("Secret Key:"), this->secretKeyLineEdit);

    awsPage->setLayout(awsLayout);
    this->pages->addWidget(awsPage);

    // General dialog layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("OK"));
    QObject::connect(okButton, &QPushButton::clicked, this, &PreferencesDialog::accept);
    buttonLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QObject::connect(cancelButton, &QPushButton::clicked, this, &PreferencesDialog::reject);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(pageList);
    layout->addWidget(this->pages);
    layout->addLayout(buttonLayout);

    this->setLayout(layout);
}

void PreferencesDialog::selectFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this->terminalFont, this,
            tr("Select a terminal font..."), QFontDialog::MonospacedFonts);

    qDebug() << "Font: " << font;
    QFontInfo fi(font);
    if (ok == true && !fi.fixedPitch()) {
        font = QFont("Courier", 12);
    }

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

void PreferencesDialog::pageSelectionChanged()
{
    this->pages->setCurrentIndex(this->pageList->currentRow());
}
