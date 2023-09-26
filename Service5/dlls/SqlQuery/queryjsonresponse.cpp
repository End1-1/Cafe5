#include "queryjsonresponse.h"
#include <QJsonArray>

#define qListOfTask 1
#define qListOfManager 2
#define qListOfEmployee 3
#define qListOfWorks 4
#define qListOfTaskWorks 5
#define qAddWorkToTask 6
#define qEmployesOfDay 7
#define qAddWorkerToDay 8
#define qChangeQty 9
#define qRemoveWork 10
#define qRemoveWorker 11
#define qWorkDetails 12
#define qWorkDetailsList 13
#define qWorkDefailsUpdate 14
#define qWorkDefailsDone 15
#define qWorkDefailsDoneUpdate 16
#define qRemoveWorkDetails 17
#define qWorkDetailsUndone 19

QueryJsonResponse::QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["kStatus"] = 1;
    fJsonOut["kData"] = "";

    switch (fJsonIn["query"].toInt()) {
        case qListOfTask:
        listOfTask();
        break;
    case qListOfManager:
        listOfManager();
        break;
    case qListOfEmployee:
        listOfEmployee();
        break;
    case qListOfWorks:
        listOfWorks();
        break;
    case qListOfTaskWorks:
        listOfTaskWorks();
        break;
    case qAddWorkToTask:
        addWorkToTask();
        break;
    case qEmployesOfDay:
        employesOfDay();
        break;
    case qAddWorkerToDay:
        addWorkerToDay();
        break;
    case qChangeQty:
        changeQty();
        break;
    case qRemoveWork:
        removeWork();
        break;
    case qRemoveWorker:
        removeWorker();
        break;
    case qWorkDetails:
        workDetails();
        break;
    case qWorkDetailsList:
        workDetailsList();
        break;
    case qWorkDefailsUpdate:
        workDetailsUpdate();
        break;
    case qWorkDefailsDone:
        workDetailsDone();
        break;
    case qWorkDefailsDoneUpdate:
        workDetailsDoneUpdate();
        break;
    case qRemoveWorkDetails:
        removeWorkDetails();
        break;
    case qWorkDetailsUndone:
        workDetailsUndone();
        break;
    default:
        fJsonOut["kData"] = "Unknown query";
        fJsonOut["kStatus"] = 4;
        break;
    }
}

void QueryJsonResponse::dbToArray(QJsonArray &ja)
{
    while (fDb.next()) {
        QJsonObject jo;
        for (int i = 0; i < fDb.columnCount(); i++){
            switch (fDb.value(i).type()) {
            case QVariant::Int:
            case QVariant::Char:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                jo[fDb.columnName(i)] = fDb.integer(i);
                break;
            case QVariant::Double:
                jo[fDb.columnName(i)] = fDb.doubleValue(i);
                break;
            default:
                jo[fDb.columnName(i)] = fDb.string(i);
                break;
            }
        }
        ja.append(jo);
    }
}

void QueryJsonResponse::listOfTask()
{
    fDb[":f_state"] = 1;
    fDb.exec("select t.f_id, t.f_product, concat(t.f_id, ' ', p.f_name) as f_name from mf_tasks t "
            "left join mf_actions_group p on p.f_id=t.f_product   "
            "where t.f_state=:f_state ");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::listOfManager()
{
    fDb.exec("select distinct(u.f_teamlead) as f_id ,"
        "concat_ws(' ', u.f_last, u.f_first) as f_name "
        "from s_user m "
        "inner join s_user u on u.f_teamlead=m.f_id "
        "WHERE u.f_teamlead>0  "
        "order by 2 " );
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::listOfEmployee()
{
    fDb.exec("select u.f_id, u.f_teamlead, concat(u.f_last, ' ', u.f_first) as f_name  "
        "from s_user u "
        "WHERE u.f_teamlead>0 and f_state=1 "
        "order by 3 ");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::listOfWorks()
{
    QString where;
    if (fJsonIn["f_task"].toInt() > 0) {
        where += QString(" and t.f_id=%1").arg(fJsonIn["f_task"].toInt());
    }
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    if (!fDb.exec(QString("select p.f_id, pr.f_name as f_productname, ac.f_name as f_processname, "
            "p.f_qty, p.f_price, p.f_taskid, concat('#', t.f_id, ' ', pr.f_name) as f_taskdate, "
            "t.f_qty as f_goal, dp.f_qty as f_ready, p.f_process, p.f_laststep "
            "from mf_daily_process p "
            "inner join mf_actions_group pr on pr.f_id=p.f_product "
            "inner join mf_actions ac on ac.f_id=p.f_process "
            "left join mf_tasks t on t.f_id=p.f_taskid "
            "left join (select dp.f_process, dp.f_taskid, sum(dp.f_qty) as f_qty "
                "from mf_daily_process dp group by 1,2) as dp on dp.f_taskid=p.f_taskid and dp.f_process=p.f_process "
                  "where p.f_date=:f_date and p.f_worker=:f_worker %1 "
                  "order by pr.f_name, ac.f_id  ").arg(where))) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }

    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::listOfTaskWorks()
{
    fDb[":f_product"] = fJsonIn["f_product"].toInt();
    if (!fDb.exec("select p.f_id , p.f_rowid + 1 as f_rowid , p.f_product , gr.f_name as f_grname, "
                  "p.f_process , ac.f_name as f_acname, "
                  "p.f_durationsec , p.f_price , ac.f_state  "
                  "from mf_process p "
                  "inner join mf_actions_group gr on gr.f_id=p.f_product "
                  "inner join mf_actions ac on ac.f_id=p.f_process "
                  "where p.f_product = :f_product "
                  "order by gr.f_name, p.f_rowid")) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::addWorkToTask()
{
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    fDb.exec("select f_id from mf_daily_workers where f_date=:f_date and f_worker=:f_worker");
    if (fDb.next() == false) {
        fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
        fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
        fDb.insert("mf_daily_workers");
    }

    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    fDb[":f_product"] = fJsonIn["f_product"].toInt();
    fDb[":f_process"] = fJsonIn["f_process"].toInt();
    fDb[":f_qty"] = 0;
    fDb[":f_price"] = fJsonIn["f_price"].toDouble();
    fDb[":f_taskid"] = fJsonIn["f_taskid"].toInt();
    fDb[":f_laststep"] = fJsonIn["f_laststep"].toInt();
    if (!fDb.insert("mf_daily_process")) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
}

void QueryJsonResponse::employesOfDay()
{
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb.exec("select m.f_worker as f_id, u.f_teamlead, concat(u.f_last, ' ', u.f_first) as f_name "
        "from mf_daily_workers m "
        "inner join s_user u on u.f_id=m.f_worker "
        "where f_date=:f_date ");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::addWorkerToDay()
{
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    fDb.exec("select f_id from mf_daily_workers where f_date=:f_date and f_worker=:f_worker");
    if (fDb.next() == false) {
        fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
        fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
        fDb.insert("mf_daily_workers");
    }
}

void QueryJsonResponse::changeQty()
{
    fDb[":f_qty"] = fJsonIn["f_qty"].toDouble();
    fDb.update("mf_daily_process", "f_id", fJsonIn["f_id"].toInt());

    fDb[":f_id"] = fJsonIn["f_taskid"].toInt();
    fDb[":f_taskid"] = fJsonIn["f_taskid"].toInt();
    fDb[":f_process"] = fJsonIn["f_process"].toInt();
    fDb.exec("update mf_tasks set f_ready=(select sum(f_qty) from mf_daily_process where f_taskid=:f_taskid and f_process=:f_process) where f_id=:f_id");

}

void QueryJsonResponse::removeWork()
{
    fDb[":f_id"] = fJsonIn["f_id"].toInt();
    if(!fDb.exec("delete from mf_daily_process where f_Id=:f_id")) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fDb[":f_processid"] = fJsonIn["f_id"].toInt();
    fDb.exec("delete from mf_process_details_done where f_processid=:f_processid");
}

void QueryJsonResponse::removeWorker()
{
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    if (!fDb.exec("delete from mf_daily_process where f_date=:f_date and f_worker=:f_worker")) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    if (!fDb.exec("delete from mf_daily_workers where f_date=:f_date and f_worker=:f_worker")) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }
}

void QueryJsonResponse::workDetails()
{
    int id = fJsonIn["f_id"].toInt();
    fDb[":f_task"] = fJsonIn["f_task"].toInt();
    fDb[":f_process"] = fJsonIn["f_process"].toInt();
    fDb[":f_color"] = fJsonIn["f_color"].toString();
    fDb[":f_34p"] = fJsonIn["f_34"].toInt();
    fDb[":f_36p"] = fJsonIn["f_36"].toInt();
    fDb[":f_38p"] = fJsonIn["f_38"].toInt();
    fDb[":f_40p"] = fJsonIn["f_40"].toInt();
    fDb[":f_42p"] = fJsonIn["f_42"].toInt();
    fDb[":f_44p"] = fJsonIn["f_44"].toInt();
    fDb[":f_46p"] = fJsonIn["f_46"].toInt();
    fDb[":f_48p"] = fJsonIn["f_48"].toInt();
    fDb[":f_50p"] = fJsonIn["f_50"].toInt();
    fDb[":f_52p"] = fJsonIn["f_52"].toInt();
    if (id == 0) {
        fDb.insert("mf_process_details", id);
    } else {
        fDb.update("mf_process_details", "f_id", id);
    }

    int task = fJsonIn["f_task"].toInt();
    fDb[":f_task"] = task;
    fDb.exec("select sum(f_34p+f_36p+f_38p+f_40p+f_42p+f_44p+f_46p+f_48p+f_50p+f_52p) from mf_process_details where f_task=:f_task");
    fDb.next();
    fDb[":f_id"] = task;
    fDb[":f_qty"] = fDb.doubleValue(0);
    fDb.exec("update mf_tasks set f_qty=:f_qty where f_id=:f_id");
    fJsonOut["kData"] = QString::number(id);
}

void QueryJsonResponse::workDetailsList()
{
    int id = fJsonIn["f_taskid"].toInt();
    fDb[":f_taskid"] = id;
    fDb.exec("select * from mf_process_details where f_task=:f_taskid");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::workDetailsUpdate()
{
    fDb[":" + fJsonIn["f_field"].toString()] = fJsonIn["f_qty"].toInt();
    fDb.update("mf_process_details", "f_id", fJsonIn["f_id"].toInt());
    fDb[":f_id"] = fJsonIn["f_id"].toInt();
    fDb.exec("select f_task, f_process from mf_process_details where f_id=:f_id");
    if (fDb.next() == false) {
        fJsonOut["kData"] = fDb.lastDbError();
        fJsonOut["kStatus"] = 4;
        return;
    }

    int task = fDb.integer("f_task");
    fDb[":f_task"] = task;
    fDb.exec("select sum(f_34p+f_36p+f_38p+f_40p+f_42p+f_44p+f_46p+f_48p+f_50p+f_52p) from mf_process_details where f_task=:f_task");
    fDb.next();
    fDb[":f_id"] = task;
    fDb[":f_qty"] = fDb.doubleValue(0);
    fDb.exec("update mf_tasks set f_qty=:f_qty where f_id=:f_id");
}

void QueryJsonResponse::workDetailsDone()
{
    fDb[":f_task"] = fJsonIn["f_task"].toInt();
    fDb[":f_process"] = fJsonIn["f_process"].toInt();
    fDb[":f_dailyid"] = fJsonIn["f_dailyid"].toInt();
    fDb.exec("SELECT pd.f_color, pdd.f_processid, "
            "pd.f_34p, "
            "pd.f_36p, "
            "pd.f_38p, "
            "pd.f_40p, "
            "pd.f_42p, "
            "pd.f_44p, "
            "pd.f_46p, "
            "pd.f_48p, "
            "pd.f_50p, "
            "pd.f_52p, "

            "coalesce(pdd.f_34d, 0) as f_34d,"
            "coalesce(pdd.f_36d, 0) AS f_36d,"
            "coalesce(pdd.f_38d, 0) AS f_38d,"
            "coalesce(pdd.f_40d, 0) AS f_40d,"
            "coalesce(pdd.f_42d, 0) AS f_42d,"
            "coalesce(pdd.f_44d, 0) AS f_44d,"
            "coalesce(pdd.f_46d, 0) AS f_46d,"
            "coalesce(pdd.f_48d, 0) AS f_48d,"
            "coalesce(pdd.f_50d, 0) AS f_50d,"
            "coalesce(pdd.f_52d, 0) AS f_52d,"

            "coalesce(pdc.f_34c, 0) as f_34c,"
            "coalesce(pdc.f_36c, 0) AS f_36c,"
            "coalesce(pdc.f_38c, 0) AS f_38c,"
            "coalesce(pdc.f_40c, 0) AS f_40c,"
            "coalesce(pdc.f_42c, 0) AS f_42c,"
            "coalesce(pdc.f_44c, 0) AS f_44c,"
            "coalesce(pdc.f_46c, 0) AS f_46c,"
            "coalesce(pdc.f_48c, 0) AS f_48c,"
            "coalesce(pdc.f_50c, 0) AS f_50c,"
            "coalesce(pdc.f_52c, 0) AS f_52c,"

            "coalesce(pdc.f_id, 0) AS f_id, 1000 as f_check "
            "FROM mf_process_details pd "

            "left join (select f_id, f_color, f_processid,"
            " sum(f_34d) as f_34d, "
            " sum(f_36d) as f_36d, "
            " sum(f_38d) as f_38d, "
            " sum(f_40d) as f_40d, "
            " sum(f_42d) as f_42d, "
            " sum(f_44d) as f_44d, "
            " sum(f_46d) as f_46d, "
            " sum(f_48d) as f_48d, "
            " sum(f_50d) as f_50d, "
            " sum(f_52d) as f_52d "
            "from mf_process_details_done where f_taskid=:f_task and f_processid in "
                "(select f_id from mf_daily_process where f_product=(select f_product from mf_tasks where f_id=:f_task) and f_process=:f_process) group by 2) "
                "pdd on pd.f_color=pdd.f_color "

            "left join (select f_id, f_color, f_processid,"
            " f_34d as f_34c, "
            " f_36d as f_36c, "
            " f_38d as f_38c, "
            " f_40d as f_40c, "
            " f_42d as f_42c, "
            " f_44d as f_44c, "
            " f_46d as f_46c, "
            " f_48d as f_48c, "
            " f_50d as f_50c, "
            " f_52d as f_52c "
            "from mf_process_details_done where f_taskid=:f_task) pdc on pd.f_color=pdc.f_color and pdc.f_processid=:f_dailyid "

            "WHERE pd.f_task=:f_task "
            "order by 1 ");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::workDetailsDoneUpdate()
{
    int id = fJsonIn["f_id"].toInt();
    if (id == 0) {
        fDb[":f_taskid"] = fJsonIn["f_taskid"].toInt();
        fDb[":f_processid"] = fJsonIn["f_dailyid"].toInt();
        fDb[":f_color"] = fJsonIn["f_color"].toString();
        fDb.insert("mf_process_details_done", id);
    }
    fDb[":f_id"] = id;
    fDb[":f_qty"] = fJsonIn["f_qty"].toInt();
    fDb[":" + fJsonIn["f_field"].toString() + "d"] = fJsonIn["f_qty"].toInt();
    fDb.exec("update mf_process_details_done set " +fJsonIn["f_field"].toString() + "d" +"=coalesce(" +fJsonIn["f_field"].toString() + "d, 0)+:f_qty where f_id=:f_id");

    fDb[":f_id"] = fJsonIn["f_dailyid"].toInt();
    fDb[":f_qty"] = fJsonIn["f_qty"].toInt();
    fDb.exec("update mf_daily_process set f_qty=f_qty+:f_qty where f_id=:f_id");
    fJsonOut["kData"] = QString::number(id);
}

void QueryJsonResponse::removeWorkDetails()
{
    fDb[":f_taskid"] = fJsonIn["f_task"].toInt();
    fDb[":f_color"] = fJsonIn["f_color"].toString();
    fDb.exec("delete from mf_process_details where f_task=:f_taskid and f_color=:f_color");
}

void QueryJsonResponse::workDetailsUndone()
{
    int id = fJsonIn["f_id"].toInt();
    if (id == 0) {
        fDb[":f_taskid"] = fJsonIn["f_taskid"].toInt();
        fDb[":f_processid"] = fJsonIn["f_dailyid"].toInt();
        fDb[":f_color"] = fJsonIn["f_color"].toString();
        fDb.insert("mf_process_details_done", id);
    }
    fDb[":" + fJsonIn["f_field"].toString() + "d"] = 0;
    fDb.update("mf_process_details_done", "f_id", id);

    fDb[":f_id"] = fJsonIn["f_dailyid"].toInt();
    fDb[":f_qty"] = fJsonIn["f_qty"].toInt();
    fDb.exec("update mf_daily_process set f_qty=f_qty-:f_qty where f_id=:f_id");
}
