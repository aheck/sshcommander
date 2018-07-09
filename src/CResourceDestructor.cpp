#include "CResourceDestructor.h"

CResourceDestructor::CResourceDestructor(std::function<void (void)> cleanupCode)
{
    this->cleanupCode = cleanupCode;
}

CResourceDestructor::~CResourceDestructor()
{
    this->cleanupCode();
}
