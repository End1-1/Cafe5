#include "queryjsonresponse.h"
#include <QJsonArray>
#include <QJsonDocument>

QueryJsonResponse::QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["kStatus"] = 1;
    fJsonOut["kAction"] = fJsonIn["action"].toInt();
    fJsonOut["kData"] = "";

    if (fJsonIn["action"].toInt() == 0) {
        fJsonOut["kStatus"] = 4;
        fJsonOut["kData"] = "Unknown query";
        return;
    }

    switch (fJsonIn["action"].toInt()) {
    case 1:
        getList();
        break;
    case 2:
        updateState();
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

bool QueryJsonResponse::getList()
{
    QString sql = QString("select r.record_id,"
        "lpad(extract(hour from cast(r.date_register as time)),2,'0') || ':' || lpad(extract(minute from cast(r.date_register as time)),2,'0') as reg_time, "
        "t.name as table_name, r.state_id, "
        "e.lname || ' ' || e.fname as staff_name, d.name as dish_name, r.qty, "
        "od.comments "
        "from o_dishes_reminder r "
        "inner join h_table t on t.id=r.table_id "
        "inner join employes e on e.id=r.staff_id "
        "inner join me_dishes d on d.id=r.dish_id "
        "inner join o_dishes od on od.id=r.record_id "
        "where r.state_id in (0,1,2) and r.reminder_id=%2 "
        "order by r.id ").arg(QString::number(fJsonIn["reminder"].toInt()));
    if (fDb.exec(sql) == false) {
        return returnFail(fDb.lastDbError());
    }
    QJsonArray ja;
    dbToArray(ja);
    fJsonOut["kData"] = ja;
    return true;
}

bool QueryJsonResponse::updateState()
{
    fDb[":record_id"] = fJsonIn["record_id"].toInt();
    fDb.exec("select state_id from o_dishes_reminder where record_id=:record_id");
    if (fDb.next() == false) {
        return returnFail("Invalid record id");
    }
    int currstate = fDb.integer("state_id");
    fDb[":state_id"] = fJsonIn["state_id"].toInt();
    switch (fJsonIn["state_id"].toInt()) {
    case 2:
        fDb[":date_start"] = QDateTime::currentDateTime();
        break;
    case 3:
        if (currstate == 1) {
            fDb[":date_start"] = QDateTime::currentDateTime();
        }
        fDb[":date_ready"] = QDateTime::currentDateTime();
        break;
    case 5:
        fDb[":date_removed"] = QDateTime::currentDateTime();
        break;
    }
    if (fDb.update("o_dishes_reminder", "record_id", fJsonIn["record_id"].toInt()) == false) {
        return returnFail(fDb.lastDbError());
    }
    return true;
}

bool QueryJsonResponse::returnFail(const QString &err)
{
    fJsonOut["kStatus"] = 4;
    fJsonOut["kData"] = err;
    return false;
}
