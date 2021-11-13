#include "databaseconnectionmanager.h"
#include "configini.h"

QHash<QString, DatabaseConnection> DatabaseConnectionManager::fDatabaseConnections;

DatabaseConnectionManager::DatabaseConnectionManager()
{

}

bool DatabaseConnectionManager::init()
{
    DatabaseConnection dc;
    dc.name = SYSTEM;
    dc.host = ConfigIni::value("db/host");
    dc.schema = ConfigIni::value("db/schema");
    dc.username = ConfigIni::value("db/username");
    dc.password = ConfigIni::value("db/password");
    fDatabaseConnections.insert(SYSTEM, dc);
    Database db;
    JsonHandler jh;
    if (!DatabaseConnectionManager::openDatabase(db, jh)) {
        db.close();
        return false;
    }
    db.exec("select * from system_databases");
    while (db.next()) {
        dc.name = db.stringValue("fname");
        dc.host = db.stringValue("fhost");
        dc.schema = db.stringValue("fschema");
        dc.username = db.stringValue("fusername");
        dc.password = db.stringValue("fpassword");
        fDatabaseConnections.insert(dc.name, dc);
    }
    return true;
}

bool DatabaseConnectionManager::openDatabase(Database &db, JsonHandler &jh)
{
    return openDatabase(SYSTEM, db, jh);
}

bool DatabaseConnectionManager::openDatabase(const QString &name, Database &db, JsonHandler &jh)
{
    Q_ASSERT(fDatabaseConnections.contains(name));
    DatabaseConnection &dc = fDatabaseConnections[name];
    if (!db.open(dc.host, dc.schema, dc.username, dc.password)) {
        jh["message"] = "Database error: " + db.lastDbError();
        return false;
    }
    return true;
}
