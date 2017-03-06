#include "Preferences.h"

Preferences::Preferences()
{
    this->hasDefaultTerminalFont = false;
}

Preferences::~Preferences()
{
    this->hasDefaultTerminalFont = false;
}

Preferences& Preferences::getInstance()
{
    static Preferences preferences;
    return preferences;
}

void Preferences::read()
{
    QSettings settings;

    settings.beginGroup("AWS");
    this->setAWSAccessKey(settings.value("accessKey", "").toString());
    this->setAWSSecretKey(settings.value("secretKey", "").toString());
    settings.endGroup();

    settings.beginGroup("Terminal");
    QString terminalFontStr = settings.value("terminalFont", "").toString();
    QFont terminalFont;

    if (terminalFontStr.isEmpty()) {
        terminalFont = this->getDefaultTerminalFont();
    } else {
        terminalFont.fromString(terminalFontStr);
    }

    this->setTerminalFont(terminalFont);

    this->setColorScheme(settings.value("colorScheme", "Linux").toString());
    settings.endGroup();
}

void Preferences::save()
{
    QSettings settings;

    settings.beginGroup("Terminal");
    settings.setValue("terminalFont", this->getTerminalFont().toString());
    settings.setValue("colorScheme", this->getColorScheme());
    settings.endGroup();

    settings.beginGroup("AWS");
    settings.setValue("accessKey", this->getAWSAccessKey());
    settings.setValue("secretKey", this->getAWSSecretKey());
    settings.endGroup();
}

QFont Preferences::getDefaultTerminalFont()
{
    if (!this->hasDefaultTerminalFont) {
        QTermWidget *console = new QTermWidget(0);
        this->defaultTerminalFont = console->getTerminalFont();
        this->hasDefaultTerminalFont = true;
        delete console;
    }

    return this->defaultTerminalFont;
}

QFont Preferences::getTerminalFont()
{
    return this->terminalFont;
}

void Preferences::setTerminalFont(const QFont &font)
{
    this->terminalFont = font;
}

const QString Preferences::getAWSAccessKey()
{
    return this->awsAccessKey;
}

void Preferences::setAWSAccessKey(const QString accessKey)
{
    this->awsAccessKey = accessKey;
}

const QString Preferences::getAWSSecretKey()
{
    return this->awsSecretKey;
}

void Preferences::setAWSSecretKey(const QString secretKey)
{
    this->awsSecretKey = secretKey;
}

const QString Preferences::getColorScheme()
{
    return this->colorScheme;
}

void Preferences::setColorScheme(const QString colorScheme)
{
    this->colorScheme = colorScheme;
}
