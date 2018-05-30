#include "DirEntry.h"

DirEntry::DirEntry()
{
    this->hPermissions = false;
    this->permissions = 0;

    this->hUid = false;
    this->uid = 0;

    this->hGid = false;
    this->gid = 0;

    this->hAtime = false;
    this->atime = 0;

    this->hMtime = false;
    this->mtime = 0;
}

void DirEntry::setLongEntry(QString longEntry)
{
    this->longEntry = longEntry;

    QRegExp space("\\s+");
    QStringList fields = longEntry.split(space);
    if (fields.length() < 3) {
        return;
    }

    this->user = fields.at(2);
}

QString DirEntry::getUser()
{
    return this->user;
}

QString DirEntry::getFilename()
{
    return this->filename;
}

void DirEntry::setFilename(QString filename)
{
    this->filename = filename;
}

QString DirEntry::getPath()
{
    return this->path;
}

void DirEntry::setPath(QString path)
{
    this->path = path;
}

bool DirEntry::hasPermissions()
{
    return this->hPermissions;
}

void DirEntry::setPermissions(uint64_t permissions)
{
    this->hPermissions = true;
    this->permissions = permissions;
}

QString DirEntry::getPermissionsString()
{
    QString s;

    if (LIBSSH2_SFTP_S_ISLNK(this->permissions)) s.append("l");
    if (LIBSSH2_SFTP_S_ISCHR(this->permissions)) s.append("c");
    if (LIBSSH2_SFTP_S_ISDIR(this->permissions)) s.append("d");
    if (LIBSSH2_SFTP_S_ISBLK(this->permissions)) s.append("b");
    if (LIBSSH2_SFTP_S_ISREG(this->permissions)) s.append("-");
    if (LIBSSH2_SFTP_S_ISFIFO(this->permissions)) s.append("f");
    if (LIBSSH2_SFTP_S_ISSOCK(this->permissions)) s.append("s");

    if (this->permissions & LIBSSH2_SFTP_S_IRUSR) s.append("r"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IWUSR) s.append("w"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IXUSR) s.append("x"); else s.append("-");

    if (this->permissions & LIBSSH2_SFTP_S_IRGRP) s.append("r"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IWGRP) s.append("w"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IXGRP) s.append("x"); else s.append("-");

    if (this->permissions & LIBSSH2_SFTP_S_IROTH) s.append("r"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IWOTH) s.append("w"); else s.append("-");
    if (this->permissions & LIBSSH2_SFTP_S_IXOTH) s.append("x"); else s.append("-");

    return s;
}

bool DirEntry::isSymLink()
{
    return LIBSSH2_SFTP_S_ISLNK(this->permissions);
}

bool DirEntry::isRegularFile()
{
    return LIBSSH2_SFTP_S_ISREG(this->permissions);
}

bool DirEntry::isDirectory()
{
    return LIBSSH2_SFTP_S_ISDIR(this->permissions);
}

bool DirEntry::isCharacterDevice()
{
    return LIBSSH2_SFTP_S_ISCHR(this->permissions);
}

bool DirEntry::isBlockDevice()
{
    return LIBSSH2_SFTP_S_ISBLK(this->permissions);
}

bool DirEntry::isFIFO()
{
    return LIBSSH2_SFTP_S_ISFIFO(this->permissions);
}

bool DirEntry::isSocket()
{
    return LIBSSH2_SFTP_S_ISSOCK(this->permissions);
}

bool DirEntry::hasUid()
{
    return this->hUid;
}

void DirEntry::setUid(uint64_t uid)
{
    this->hUid = true;
    this->uid = uid;
}

uint64_t DirEntry::getUid()
{
    return this->uid;
}

bool DirEntry::hasGid()
{
    return this->hGid;
}

void DirEntry::setGid(uint64_t gid)
{
    this->hGid = true;
    this->gid = gid;
}

uint64_t DirEntry::getGid()
{
    return this->gid;
}

bool DirEntry::hasAtime()
{
    return this->hAtime;
}

void DirEntry::setAtime(uint64_t atime)
{
    this->hAtime = true;
    this->atime = atime;
}

uint64_t DirEntry::getAtime()
{
    return this->atime;
}

bool DirEntry::hasMtime()
{
    return this->hMtime;
}

void DirEntry::setMtime(uint64_t mtime)
{
    this->hMtime = true;
    this->mtime = mtime;
}

uint64_t DirEntry::getMtime()
{
    return this->mtime;
}

void DirEntry::setFilesize(uint64_t filesize)
{
    this->filesize = filesize;
}

uint64_t DirEntry::getFilesize()
{
    return this->filesize;
}
