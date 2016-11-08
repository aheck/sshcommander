#include "SSHConnectionEntry.h"

SSHConnectionEntry::SSHConnectionEntry()
{
    this->nextSessionNumber = 1;
    this->args = NULL;
    this->tabs = NULL;
    this->awsInstance = NULL;
}

SSHConnectionEntry::~SSHConnectionEntry()
{
    delete this->tabs;
}
