#include "SSHConnectionEntry.h"

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
    this->args = nullptr;
    this->tabs = nullptr;
    this->isAwsInstance = false;
}

SSHConnectionEntry::~SSHConnectionEntry()
{
    delete this->tabs;
}
