#include "databaseconnectionmanager.h"
#include "configini.h"
#include "logwriter.h"

QHash<QString, DatabaseConnection> DatabaseConnectionManager::fDatabaseConnections;
bool DatabaseConnectionManager::nodb = false;

DatabaseConnectionManager::DatabaseConnectionManager()
{

}

bool DatabaseConnectionManager::init(const QString &configFile)
{
    if (!configFile.isEmpty()) {
        ConfigIni::init(configFile);
    }
    if (ConfigIni::value("db/nodb") == "true") {
        nodb =true;
        return true;
    }
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
        dc.name = db.string("fname");
        dc.host = db.string("fhost");
        dc.schema = db.string("fschema");
        dc.username = db.string("fusername");
        dc.password = db.string("fpassword");
        fDatabaseConnections.insert(dc.name, dc);
    }
    return true;
}

bool DatabaseConnectionManager::openSystemDatabase(Database &db)
{
    DatabaseConnection &dc = fDatabaseConnections[SYSTEM];
    return db.open(dc.host, dc.schema, dc.username, dc.password);
}

bool DatabaseConnectionManager::openDatabase(Database &db, JsonHandler &jh)
{
    return openDatabase(SYSTEM, db, jh);
}

bool DatabaseConnectionManager::openDatabase(const QString &name, Database &db, JsonHandler &jh)
{
#ifdef QT_DEBUG
    Q_ASSERT(fDatabaseConnections.contains(name));
#else
    if (!fDatabaseConnections.contains(name)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("The database with name '%1' is not exist.").arg(name));
        jh["message"] = "Database error: " + QString("The database with name '%1' is not exist.").arg(name);
        return false;
    }
#endif
    DatabaseConnection &dc = fDatabaseConnections[name];
    if (!db.open(dc.host, dc.schema, dc.username, dc.password)) {
        jh["message"] = "Database error: " + db.lastDbError();
        return false;
    }
    return true;
}
