#pragma once

#include <QJsonObject>
#include <QString>

class ArcusClient
{
public:
    static bool chargeCard(const QString &address,
                           int port,
                           const QString &apiKey,
                           qint64 amountMinor,
                           QJsonObject &response,
                           QString &error);
};
