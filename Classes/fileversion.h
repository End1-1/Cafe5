#ifndef FILEVERSION_H
#define FILEVERSION_H

#include <QString>

class FileVersion
{
public:
    static QString getVersionString(QString fName);
    static QString getVersionString();
#ifdef Q_OS_LINUX
    static QString getElfVersion(const QString &fName);
    static QString getVersionFromProcess(const QString &fName);
    static QString getVersionFromPackageManager(const QString &fName);
#endif

private:
    FileVersion();
};

#endif // FILEVERSION_H
