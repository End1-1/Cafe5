#ifndef SERVERPRINTING_H
#define SERVERPRINTING_H

#include <QJsonObject>

class ServerPrinting
{
public:
    ServerPrinting(const QJsonObject &data);

    bool print();

private:
    QJsonObject mData;
};

#endif // SERVERPRINTING_H
