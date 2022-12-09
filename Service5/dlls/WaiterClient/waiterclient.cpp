#include "waiterclient.h"
#include "commandline.h"
#include "database.h"
#include "logwriter.h"
#include "networktable.h"
#include "ops.h"
#include "login.h"
#include "halls.h"
#include "dishes.h"
#include "order.h"
#include <QSettings>

bool dllfunc(const QByteArray &in, RawMessage &rm){
    rm.putUByte(2);
    quint32 op;
    rm.readUInt(op, in);
    rm.putUInt(op);
    QString dbname;
    rm.readString(dbname, in);

    CommandLine cl;
    QString configFile;
    cl.value("--config", configFile);
    QSettings s(configFile, QSettings::IniFormat);
    LogWriter::write(LogWriterLevel::verbose, rm.property("session").toString(),QString("rwshop. config: %1").arg(configFile));
    Database db;
    if (db.open(s.value("db/host").toString(), s.value("db/schema").toString(), s.value("db/username").toString(), s.value("db/password").toString()) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        LogWriter::write(LogWriterLevel::errors, rm.property("session").toString(), db.lastDbError());
        return false;
    }
    db[":fname"] = dbname;
    db.exec("select * from system_databases where fname=:fname");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Unknown database name");
        return false;
    }
    Database dbw;
    if (dbw.open(db.string("fhost"), db.string("fschema"), db.string("fusername"), db.string("fpassword")) == false) {
        rm.putUByte(0);
        rm.putString(dbw.lastDbError());
        return false;
    }

    switch (op) {
    case op_login:
        loginUsernamePassword(rm, dbw, in);
        break;
    case op_get_hall_list:
        loadHalls(rm, dbw, in);
        break;
    case op_get_table_list:
        loadTables(rm,  dbw, in);
        break;
    case op_get_dish_part1_list:
        loadPart1(rm, dbw, in);
        break;
    case op_get_dish_part2_list:
        loadPart2(rm, dbw, in);
        break;
    case op_login_passhash:
        loginPasswordHash(rm, dbw, in);
        break;

        /*ORDER COMMANDS */
    case op_open_table:
        openTable(rm, dbw, in);
        break;
    case op_unlock_table:
        unlockTable(rm, dbw, in);
        break;
    case op_car_list:
        loadCarsModels(rm, dbw, in);
        break;
    case op_search_platenumber:
        searchPlateNumber(rm, dbw, in);
        break;
    default:
        rm.putUByte(0);
        rm.putString(QString("Not implemented: %1").arg(op));
        return false;
    }
    return true;
}
