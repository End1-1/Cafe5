#include "rwmftasks.h"
#include "commandline.h"
#include "database.h"
#include "logwriter.h"
#include "networktable.h"
#include <QSettings>

#define op_get_task_list 1
#define op_get_products 2
#define op_create_task 3
#define op_load_task 4
#define op_get_employes 5
#define op_get_processes 6
#define op_save_process 7
#define op_update_product_progress 8
#define op_get_storage_list 9
#define op_get_workshop_list 10
#define op_get_stages_list 11
#define op_set_workshop 12
#define op_activate_process 13
#define op_load_workshop 14
#define op_load_workshop_detail  15
#define op_save_task_note 16
#define op_add_task_output 17
#define op_list_tasks_output 18
#define op_remove_task_output 19

void getListOfTask(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT t.f_id AS `NN`, date_format(t.f_datecreate, '%d/%m/%Y') AS `Օր`, m.f_name AS `Արտադրանք`, "
               "t.f_qty AS `Քանակ`, t.f_ready AS `Վիճակ`, t.f_out as `Ելք`, s.f_name as `Արտադրամաս` "
                "FROM mf_tasks t "
                "LEFT JOIN mf_actions_group m ON m.f_id=t.f_product "
                "left join mf_stage s on s.f_id=t.f_workshop "
                "WHERE t.f_state=1 ");
}

void getListOfProducts(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_name from mf_actions_group order by f_name");
}

void getListOfWorkshop(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_name from mf_stage order by f_name");
}

void getListOfStages(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_name from mf_actions_state order by f_name");
}

void getListOfStorage(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_name from c_storages order by f_name");
}

void createTask(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 product;
    quint32 workshop;
    quint32 stage;
    double qty;
    rm.readUInt(product, in);
    rm.readDouble(qty, in);
    rm.readUInt(workshop, in);
    rm.readUInt(stage, in);
    db[":f_state"] = 1;
    db[":f_datecreate"] = QDate::currentDate();
    db[":f_timecreate"] = QTime::currentTime();
    db[":f_user"] = 1;
    db[":f_product"] = product;
    db[":f_qty"] = qty;
    db[":f_ready"] = 0;
    db[":f_workshop"] = workshop;
    db[":f_stage"] = stage;
    int taskid;
    if (!db.insert("mf_tasks", taskid)){
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    rm.putUByte(1);
    rm.putUInt(taskid);
}

void loadTask(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskId;
    rm.readUInt(taskId, in);
    db[":f_id"] = taskId;
    db.exec("select t.f_datecreate , t.f_timecreate , t.f_qty, "
            "g.f_name as f_productname, t.f_ready, t.f_workshop, t.f_stage, t.f_ready, t.f_out, t.f_notes "
            "from mf_tasks t "
            "left join mf_actions_group g on g.f_id=t.f_product "
            "where t.f_id=:f_id ");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Invalid task id" + db.lastDbError());
        return;
    }
    rm.putUByte(1);
    rm.putString(db.date("f_datecreate").toString("dd/MM/yyyy"));
    rm.putString(db.time("f_timecreate").toString("HH:mm"));
    rm.putDouble(db.doubleValue("f_qty"));
    rm.putString(db.string("f_productname"));
    rm.putUInt(db.integer("f_workshop"));
    rm.putUInt(db.integer("f_stage"));
    rm.putDouble(db.doubleValue("f_ready"));
    rm.putDouble(db.doubleValue("f_out"));
    rm.putString(db.string("f_notes"));

    NetworkTable taskdesc(rm, db);
    db[":f_taskid"] = taskId;
    taskdesc.execSQL("SELECT p.f_process, s.f_name as `Փուլ`, a.f_name as `Գործողություն`, p.f_durationsec as `Տևող․`, "
                    " p.f_price as `Գին`, "
                    "t.f_qty as `Պահանջ`, coalesce(d.f_qty, 0) AS `Կատ․`,  coalesce(d.f_qty, 0) / t.f_qty * 100 as '%' "
                    "FROM mf_process p "
                    "INNER JOIN mf_actions a ON a.f_id=p.f_process "
                    "left join mf_actions_state s on s.f_id=a.f_state "
                    "INNER JOIN mf_tasks t ON t.f_product=p.f_product "
                    "LEFT JOIN (SELECT f_process, SUM(f_qty) AS f_qty FROM mf_daily_process WHERE f_taskid=:f_taskid GROUP BY f_process) d ON d.f_process=p.f_process "
                    "WHERE t.f_id=:f_taskid "
                    "ORDER BY p.f_rowid");

}

void loadWorkshop(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 workshop;
    rm.readUInt(workshop, in);
    if (workshop == 0) {
        rm.putUByte(0);
        rm.putString("No workshop selected");
        return;
    }

    NetworkTable w(rm, db);
    db[":f_workshop"] = workshop;
    w.execSQL("SELECT t.f_id, w.f_name as wfname, a.f_name as afname, "
            "date_format(t.f_datecreate, '%d/%m/%Y')as f_date, t.f_qty, t.f_ready, t.f_out, s.f_name as sfnameelina, "
            "coalesce(tt.f_ttqty*t.f_qty, 0) AS f_qtyneeded, coalesce(su.f_taskqty, 0), "
            "format(coalesce(su.f_taskqty, 0) / coalesce(tt.f_ttqty*t.f_qty, 0) *100, 0) AS f_cmpt "
            "FROM mf_tasks t "
            "LEFT join mf_actions_group a ON a.f_id=t.f_product "
            "LEFT JOIN mf_actions_state s ON s.f_id=t.f_stage "
            "LEFT JOIN mf_stage w ON w.f_id=t.f_workshop "
            "LEFT JOIN (SELECT f_product, count(f_id) AS f_ttqty FROM mf_process GROUP BY 1) tt ON tt.f_product=t.f_product "
            "left join (select f_taskid, sum(f_qty) as f_taskqty from mf_daily_process group by 1) su on su.f_taskid=t.f_id "
            "where t.f_workshop=:f_workshop");

}

void loadWorkshopDetail(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 workshop;
    rm.readUInt(workshop, in);
    if (workshop == 0) {
        rm.putUByte(0);
        rm.putString("No workshop selected");
        return;
    }

    NetworkTable w(rm, db);
    db[":f_workshop"] = workshop;
    w.execSQL("SELECT ts.f_task, date_format(ts.f_date, '%d/%m/%Y')as f_date, s.f_name, "
            "FORMAT(coalesce(dp.f_cmpt, 0) / (coalesce(sn.f_needed, 1)*t.f_qty) * 100, 0) AS f_percent,"
            "coalesce(sn.f_needed, 0), coalesce(dp.f_cmpt, 0) "
            "FROM mf_task_stage ts "
            "LEFT JOIN mf_tasks t ON t.f_id=ts.f_task "
            "LEFT JOIN mf_actions_state s ON s.f_id=ts.f_stage "
            "LEFT JOIN (SELECT tta.f_state, ttmp.f_product, COUNT(ttmp.f_id) AS f_needed "
            "    FROM mf_process ttmp LEFT JOIN mf_actions tta ON tta.f_id=ttmp.f_process GROUP BY 1, 2) sn on t.f_product=sn.f_product AND sn.f_state=ts.f_stage "
            "LEFT JOIN (SELECT tta.f_state, ttdp.f_taskid, SUM(ttdp.f_qty) AS f_cmpt "
            "    FROM mf_daily_process ttdp LEFT JOIN mf_actions tta ON tta.f_id=ttdp.f_process GROUP BY 1,2) dp ON dp.f_state=ts.f_stage AND dp.f_taskid=t.f_id "
            "WHERE t.f_workshop=:f_workshop "
            "ORDER BY 1, 2");

}



void getListOfEmployes(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_group, concat(f_last, ' ', f_first) from s_user where f_group in (11,12,13,14,15,16)");    ;
}

void getListOfProcesses(RawMessage &rm, Database &db, const QByteArray &in) {
    NetworkTable nt(rm, db);
    nt.execSQL("SELECT f_id, f_name from mf_actions");    ;
}

void setWorkshop(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskid, workshop;
    rm.readUInt(taskid, in);
    rm.readUInt(workshop, in);
    if (taskid == 0 || workshop == 0) {
        rm.putUByte(0);
        rm.putString(QString("Incorrect input params %1 %2").arg(taskid).arg(workshop));
        return;
    }
    db[":f_id"] = taskid;
    db[":f_workshop"] = workshop;
    if (!db.exec("update mf_tasks set f_workshop=:f_workshop where f_id=:f_id")) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    db[":f_task"] = taskid;
    db[":f_workshop"] = workshop;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db.insert("mf_task_workshop");
    rm.putUByte(1);
    rm.putUInt(workshop);
}

void setCurrentState(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskid, state;
    rm.readUInt(taskid, in);
    rm.readUInt(state, in);
    if (taskid == 0 || state == 0) {
        rm.putUByte(0);
        rm.putString(QString("Incorrect input params %1 %2").arg(taskid).arg(state));
        return;
    }
    db[":f_id"] = state;
    db.exec("select f_state from mf_actions where f_id=:f_id");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    state = db.integer("f_state");
    db[":f_id"] = taskid;
    db[":f_stage"] = state;
    if (!db.exec("update mf_tasks set f_stage=:f_stage where f_id=:f_id")) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    db[":f_task"] = taskid;
    db[":f_stage"] = state;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db.insert("mf_task_stage");
    rm.putUByte(1);
    rm.putUInt(state);
}

void saveProcess(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskId;
    quint32 processId;
    quint32 employeeId;
    double qty;
    double price;
    QString datestr;
    QDate date;
    rm.readUInt(taskId, in);
    rm.readUInt(processId, in);
    rm.readUInt(employeeId, in);
    rm.readDouble(qty, in);
    rm.readDouble(price, in);
    rm.readString(datestr, in);
    date = QDate::fromString(datestr, "dd/MM/yyyy");

    db[":f_id"] = taskId;
    db.exec("select * from mf_tasks where f_id=:f_id");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Incorrect task number." + db.lastDbError());
        return;
    }
    int productId = db.integer("f_product");

    db[":f_date"] = date;
    db[":f_worker"] = employeeId;
    db.exec("select * from mf_daily_workers where f_date=:f_date and f_worker=:f_worker");
    if (db.next() == false) {
        db[":f_date"] = date;
        db[":f_worker"] = employeeId;
        db.insert("mf_daily_workers");
    }

    db[":f_date"] = date;
    db[":f_taskid"] = taskId;
    db[":f_worker"] = employeeId;
    db[":f_product"] = productId;
    db[":f_process"] = processId;
    db[":f_qty"] = qty;
    db[":f_price"] = price;
    int result = db.insert("mf_daily_process");
    if (result == 0) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    rm.putUByte(1);
    rm.putUInt(result);
}

void saveTaskNotes(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskid;
    QString notes;
    rm.readUInt(taskid, in);
    rm.readString(notes, in);

    db[":f_notes"] = notes;
    if (db.update("mf_tasks", "f_id", taskid) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    rm.putUByte(1);
}

void addTaskOutput(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskid;
    double qty;
    rm.readUInt(taskid, in);
    rm.readDouble(qty, in);

    db[":f_task"] = taskid;
    db[":f_qty"] = qty;
    db[":f_date"] = QDate::currentDate();
    if (db.insert("mf_tasks_output") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }

    db[":f_id"] = taskid;
    db[":f_qty"] = qty;
    db.exec("update mf_tasks set f_out=f_out+:f_qty where f_id=:f_id");

    rm.putUByte(1);
}

void removeTaskOutput(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 rowid;
    rm.readUInt(rowid, in);

    db[":f_id"] = rowid;
    db.exec("select f_qty, f_task from mf_tasks_output where f_id=:f_id");
    db.next();
    double qty = db.doubleValue("f_qty");
    int taskid = db.integer("f_task");

    db[":f_id"] = taskid;
    db[":f_qty"] = qty;
    db.exec("update mf_tasks set f_out=f_out-:f_qty where f_id=:f_id");

     db[":f_id"] = rowid;
    if (db.exec("delete from mf_tasks_output where f_id=:f_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }

    rm.putUByte(1);
}

void loadTaskOutput(RawMessage &rm, Database &db, const QByteArray &in) {
    quint32 taskid;
    rm.readUInt(taskid, in);

    db[":f_id"] = taskid;
    if (db.exec("select t.f_qty, t.f_ready, t.f_out, p.f_name "
                "from mf_tasks t "
                "left join mf_actions_group p on p.f_id=t.f_product "
                "where t.f_id=:f_id") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return;
    }
    db.next();
    rm.putUByte(1);
    rm.putString(db.string("f_name"));
    rm.putDouble(db.doubleValue("f_qty"));
    rm.putDouble(db.doubleValue("f_ready"));
    rm.putDouble(db.doubleValue("f_out"));

    NetworkTable nt(rm, db);
    db[":f_task"] = taskid;
    nt.execSQL("SELECT f_id, f_date, f_qty from mf_tasks_output where f_task=:f_task");


}

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
    case op_get_task_list:
        getListOfTask(rm, dbw, in);
        break;
    case op_get_products:
        getListOfProducts(rm, dbw, in);
        break;
    case op_create_task:
        createTask(rm, dbw, in);
        break;
    case op_load_task:
        loadTask(rm, dbw, in);
        break;
    case op_get_employes:
        getListOfEmployes(rm, dbw, in);
        break;
    case op_get_processes:
        getListOfProcesses(rm, dbw, in);
        break;
    case op_save_process:
        saveProcess(rm, dbw, in);
        break;
    case op_update_product_progress:
        break;
    case op_get_storage_list:
        getListOfStorage(rm, dbw, in);
        break;
    case op_get_workshop_list:
        getListOfWorkshop(rm, dbw, in);
        break;
    case op_get_stages_list:
        getListOfStages(rm, dbw, in);
        break;
    case op_set_workshop:
        setWorkshop(rm, dbw, in);
        break;
    case op_activate_process:
        setCurrentState(rm, dbw, in);
        break;
    case op_load_workshop:
        loadWorkshop(rm, dbw, in);
        break;
    case op_load_workshop_detail:
        loadWorkshopDetail(rm, dbw, in);
        break;
    case op_save_task_note:
        saveTaskNotes(rm, dbw, in);
        break;
    case op_add_task_output:
        addTaskOutput(rm, dbw, in);
        break;
    case op_remove_task_output:
        removeTaskOutput(rm, dbw, in);
        break;
    case op_list_tasks_output:
        loadTaskOutput(rm, dbw, in);
        break;
    default:
        rm.putUByte(0);
        rm.putString(QString("Nothing impemented %1").arg(op));
        return false;
    }
    return true;
}
