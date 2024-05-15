#ifndef DATABASECONNECTIONMANAGER_H
#define DATABASECONNECTIONMANAGER_H

#include "database.h"
#include "jsonhandler.h"

#define SYSTEM "system"

struct DatabaseConnection {
    QString name;
    QString host;
    QString schema;
    QString username;
    QString password;
};

class DatabaseConnectionManager
{
public:
    DatabaseConnectionManager();
    bool init(const QString &configFile);
    static bool openSystemDatabase(Database &db);
    static bool openDatabase(Database &db, JsonHandler &jh);
    static bool openDatabase(const QString &name, Database &db, JsonHandler &jh);
    static QHash<QString, DatabaseConnection> fDatabaseConnections;
};

#endif // DATABASECONNECTIONMANAGER_H
