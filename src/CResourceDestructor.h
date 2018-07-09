/*****************************************************************************
 *
 * CResourceDestructor is a class providing a destructor for handles for C
 * resources of any kind.
 *
 * Whenever you obtain a C resource handle just declare an object of this
 * class and provide the constructor with a lambda function which captures
 * your resource handle and provides some code to destroy it. This code will
 * then be executed once your CResourceDestructor object is destroyed.
 *
 ****************************************************************************/

#ifndef CRESOURCEDESTRUCTOR_H
#define CRESOURCEDESTRUCTOR_H

#include <functional>

class CResourceDestructor
{
public:
    CResourceDestructor(std::function<void (void)> cleanupCode);
    ~CResourceDestructor();

private:
    std::function <void (void)> cleanupCode;
};

#endif
