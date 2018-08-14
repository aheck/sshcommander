/*****************************************************************************
 *
 * ExternalProgramFinder is a convenience class providing static methods
 * for everything related to external programs used by SSH Commander.
 *
 ****************************************************************************/

#ifndef EXTERNALPROGRAMFINDER_H
#define EXTERNALPROGRAMFINDER_H

#include <QFile>

class ExternalProgramFinder
{

public:
    ExternalProgramFinder() = delete;

    static QString getSSHPath();
    static QString getSSHFSPath();
};

#endif
