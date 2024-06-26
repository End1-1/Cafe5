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
        taskdesc.execSQL("select id, name, menu_id, service_value from h_hall");
        break;
    case version2:
        taskdesc.execSQL("SELECT h.f_id, h.f_name, s1.f_value AS f_menu, s2.f_value AS f_service "
                        "FROM h_halls h "
                        "LEFT JOIN s_settings_values s1 ON s1.f_settings=h.f_settings AND s1.f_key=9 "
                        "LEFT JOIN s_settings_values s2 ON s2.f_settings=h.f_settings AND s2.f_key=2 ");
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
        taskdesc.execSQL("SELECT t.id, t.hall_id, coalesce(o.state_id, 0) AS f_state, t.name, coalesce(o.id, '') as f_orderid "
                        "FROM h_table t "
                        "LEFT JOIN o_order o ON o.table_id=t.id AND o.state_id=1 "
                        "ORDER BY t.hall_id, t.queue ");
        break;
    case version2:
        taskdesc.execSQL("select t.f_id, t.f_hall, coalesce(o.f_state, 0) as f_state, t.f_name, coalesce(o.f_id, '') as f_orderid "
                       "from h_tables t "
                        "left join o_header o on o.f_table=t.f_id and o.f_state=1 "
                        "order by t.f_hall, t.f_id");
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
    rm.putString("TABLES LIST EMPTY");
}

void loadCarsModels(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    rm.readUByte(version, in);
    NetworkTable taskdesc(rm, db);
    switch (version) {
    case version1:
        taskdesc.execSQL("select 1 as id, 'car' as name from rdb$database");
        break;
    case version2:
        taskdesc.execSQL("select 1 as f_id, 'car' as f_name");
        break;
    case version3:
        taskdesc.execSQL("select f_id, f_model from d_car_model order by f_model ");
        break;
    }

    if (taskdesc.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("CAR MODELS LIST EMPTY");
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
