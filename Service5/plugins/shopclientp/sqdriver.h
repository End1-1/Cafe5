#ifndef SQDRIVER_H
#define SQDRIVER_H

#include <QHash>

#define sq(x) SQDriver::get(x)

class SQDriver
{
public:
    SQDriver();
    static QString get(const QString &query_name);
    static SQDriver *fInstance;

private:
    static QHash<QString, QString> fListOfQueries;
};

#endif // SQDRIVER_H
