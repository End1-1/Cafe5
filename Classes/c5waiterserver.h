#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include <QObject>
#include <QJsonObject>
#include "c5database.h"

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer(const QJsonObject &o);
    void reply(QJsonObject &o);
private:
    const QJsonObject &fIn;
    void saveDish(const QJsonObject &h, QJsonObject &o, C5Database &db);
};

#endif // C5WAITERSERVER_H
