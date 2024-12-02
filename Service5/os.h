#ifndef OS_H
#define OS_H

#include <QString>

class OS
{
public:
    OS();
    static QString p(const QString &cmd);
};

#endif // OS_H
