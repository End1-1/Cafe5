#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QHash>

class CommandLine
{
public:
    CommandLine();
    bool value(const QString &k, QString &v);

private:
    QHash<QString, QString> fArgv;
};

#endif // COMMANDLINE_H
