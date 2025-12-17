#pragma once

#include <QWebSocket>

struct SocketStruct {
    QWebSocket* socket;
    QString tenantId;
    QString databaseName;
    int userId;
};
