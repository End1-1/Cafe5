#include "halls.h"
#include "translator.h"
#include "networktable.h"
#include "ops.h"


void loadHalls(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    rm.readUByte(version, in);
    NetworkTable taskdesc(rm, db);
    switch (version) {
    case version1:
        break;
    case version2:
        break;
    case version3:
        taskdesc.execSQL("select f_id, f_name, f_defaultmenu, f_servicevalue from r_hall");
        break;
    }

    if (taskdesc.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("HALL LIST EMPTY");
}

void loadTables(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    rm.readUByte(version, in);
    NetworkTable taskdesc(rm, db);
    switch (version) {
    case version1:
        break;
    case version2:
        break;
    case version3:
        taskdesc.execSQL("SELECT t.f_id, t.f_hall, coalesce(o.f_state, 0) AS f_state, t.f_name, coalesce(o.f_id, '') as f_orderid "
                        "FROM r_table t "
                        "LEFT JOIN o_header o ON o.f_table=t.f_id AND o.f_state=1 "
                        "ORDER BY t.f_hall, t.f_queue ");
        break;
    }

    if (taskdesc.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("HALL LIST EMPTY");
}

void loadCarsModels(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    rm.readUByte(version, in);
    NetworkTable taskdesc(rm, db);
    switch (version) {
    case version1:
        break;
    case version2:
        break;
    case version3:
        taskdesc.execSQL("select f_id, f_model from d_car_model order by f_model ");
        break;
    }

    if (taskdesc.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("HALL LIST EMPTY");
}

bool searchPlateNumber(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    rm.readUByte(version, in);
    QString licenseplate;
    rm.readString(licenseplate, in);
    
    quint32 modelid = 0, customerid = 0;
    QString name, info;
    
    db[":f_govnumber"] = licenseplate;
    switch (version) {
    case version1:
        break;
    case version2:
        break;
    case version3:
        if (db.exec("select f_model, f_costumer from o_car where lower(f_govnumber)=lower(:f_govnumber) order by f_id desc limit 1 ") == false) {
            return rmerror(rm, db.lastDbError());
        }
        if (db.next()) {
            modelid = db.integer("f_model");
            customerid = db.integer("f_costumer");
        }
        if (customerid > 0) {
            db[":f_id"] = customerid;
            db.exec("select f_name, f_info from o_debt_holder where f_id=:f_id");
            if (db.next()) {
                name = db.string("f_name");
                info = db.string("f_info");
            }
        }
        break;
    }

    rm.putUByte(1);
    rm.putUInt(modelid);
    rm.putUInt(customerid);
    rm.putString(name);
    rm.putString(info);
    return true;
}
