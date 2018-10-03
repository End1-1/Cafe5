#ifndef C5WAITERSERVER_H
#define C5WAITERSERVER_H

#include <QObject>
#include <QJsonObject>

class C5WaiterServer : public QObject
{
    Q_OBJECT
public:
    C5WaiterServer(const QJsonObject &o);
    void reply(QJsonObject &o);
private:
    const QJsonObject &fIn;
};

#endif // C5WAITERSERVER_H
