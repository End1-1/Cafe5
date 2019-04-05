#ifndef FILEVERSION_H
#define FILEVERSION_H

#include <QString>

class FileVersion
{
public:
    static QString getVersionString(QString fName);

private:
    FileVersion();
};

#endif // FILEVERSION_H
