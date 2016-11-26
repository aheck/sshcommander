#include "Preferences.h"

Preferences::Preferences()
{

}

QFont Preferences::getTerminalFont()
{
    return this->terminalFont;
}

void Preferences::setTerminalFont(const QFont &font)
{
    this->terminalFont = font;
}
