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
        "WHERE u.f_teamlead>0 "
        "order by 3 ");
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
}

void QueryJsonResponse::listOfWorks()
{
    fDb[":f_date"] = QDate::fromString(fJsonIn["f_date"].toString(), "dd/MM/yyyy");
    fDb[":f_worker"] = fJsonIn["f_worker"].toInt();
    if (!fDb.exec("select p.f_id, pr.f_name as f_productname, ac.f_name as f_processname, "
            "p.f_qty, p.f_price, p.f_taskid, concat('#', t.f_id, ' ', pr.f_name) as f_taskdate, "
            "t.f_qty as f_goal, dp.f_qty as f_ready, p.f_process, p.f_laststep "
            "from mf_daily_process p "
            "inner join mf_actions_group pr on pr.f_id=p.f_product "
            "inner join mf_actions ac on ac.f_id=p.f_process "
            "left join mf_tasks t on t.f_id=p.f_taskid "
            "left join (select dp.f_process, dp.f_taskid, sum(dp.f_qty) as f_qty "
                "from mf_daily_process dp group by 1,2) as dp on dp.f_taskid=p.f_taskid and dp.f_process=p.f_process "
                  "where p.f_date=:f_date and p.f_worker=:f_worker "
                  "order by pr.f_name, ac.f_id  ")) {
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
