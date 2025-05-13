#ifndef WAITER_H
#define WAITER_H

#include <QJsonObject>

class QWebSocket;

class Waiter
{
public:
    Waiter(const QJsonObject &jdoc);

    QString process();

private:
    QJsonObject mDoc;
};

#endif // WAITER_H
