#ifndef FILEVERSION_H
#define FILEVERSION_H

#include <QString>

class FileVersion
{
public:
    static QString getVersionString(QString fName);
    static QString getVersionString();

private:
    FileVersion();
};

#endif // FILEVERSION_H
