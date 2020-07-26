#ifndef C5LOGSYSTEM_H
#define C5LOGSYSTEM_H

#include <QObject>

#define ls(x) C5LogSystem::writeEvent(x)

class C5LogSystem : QObject
{
    Q_OBJECT

public:
    C5LogSystem();

    static QString fileName(const QDate &d);

    static void writeEvent(const QString message);
};

#endif // C5LOGSYSTEM_H
