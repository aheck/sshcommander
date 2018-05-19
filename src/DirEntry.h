/*****************************************************************************
 *
 * DirEntry represents a directory entry as retrieved with SFTP via libssh2.
 *
 ****************************************************************************/

#include <QString>

#include <libssh2_sftp.h>

class DirEntry
{
public:
    DirEntry();

    QString getFilename();
    void setFilename(QString filename);
    QString getPath();
    void setPath(QString path);

    bool hasPermissions();
    void setPermissions(uint64_t permissions);
    QString getPermissionsString();

    bool isSymLink();
    bool isRegularFile();
    bool isDirectory();
    bool isCharacterDevice();
    bool isBlockDevice();
    bool isFIFO();
    bool isSocket();

    bool hasUid();
    void setUid(uint64_t uid);
    uint64_t getUid();

    bool hasGid();
    void setGid(uint64_t gid);
    uint64_t getGid();

    bool hasAtime();
    void setAtime(uint64_t atime);
    uint64_t getAtime();

    bool hasMtime();
    void setMtime(uint64_t mtime);
    uint64_t getMtime();

    void setFilesize(uint64_t filesize);
    uint64_t getFilesize();

private:
    QString filename;
    QString path;

    bool hPermissions;
    uint64_t permissions;

    bool hUid;
    uint64_t uid;

    bool hGid;
    uint64_t gid;

    bool hAtime;
    uint64_t atime;

    bool hMtime;
    uint64_t mtime;

    uint64_t filesize;
};
