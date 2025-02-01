#include "jzstore.h"
#include "database.h"
#include "logwriter.h"
#include <QJsonDocument>

QMap<int, QMap<QString, QString> > fConnections;
QMap<int, QString> fStorages;
QMap<int, QMap<int, QString> > fAsStorageMap;

bool jzstore(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    init();
    //VALIDATE
    QString message;
    if (!jreq.contains("month")) {
        message += "The month is required.";
    } else {
        if (jreq["month"].toInt() < 0 || jreq["month"].toInt() > 11) {
            message += "Invalid month range.";
        }
    }
    if (!jreq.contains("request")) {
        message += "The type of request is missing.";
    } else {
        QStringList requests;
        QString request = jreq["request"].toString();
        requests.append("goodslist");
        requests.append("store");
        if (!requests.contains(request)) {
            message += QString("Invalid request type %1").arg(jreq["request"].toString());
        } else {
            if (request == "store") {
                if (!jreq.contains("cafe")) {
                    message += "Cafe parameter missing.";
                } else {
                    if (!fConnections.contains(jreq["cafe"].toInt())) {
                        message += "Invalid cafe parameter";
                    }
                }
                if (!jreq.contains("store")) {
                    message += "Store parameter missing.";
                } else {
                    if (!fStorages.contains(jreq["store"].toInt())) {
                        message += "Invalid store parameter";
                    }
                }
            }
        }
    }
    if (!message.isEmpty()) {
        err = message;
        return false;
    }
    //PROCESS
    QString request = jreq["request"].toString();
    if (request == "goodslist") {
        return requestGoodsGroups(jreq, jret, err);
    } else if (request == "store") {
        return requestStore(jreq, jret, err);
    }
    return true;
}

void init()
{
    if (fConnections.isEmpty()) {
        addConnection(2, QString::fromUtf8("Օպերա"), "10.10.12.1", "cafe4", "SYSDBA", "Inter_OneStep");
        addConnection(3, QString::fromUtf8("Թումանյան"), "10.10.5.1", "cafe4", "SYSDBA", "Inter_OneStep");
        addConnection(4, QString::fromUtf8("Կոմիտաս"), "10.10.11.1", "cafe4", "SYSDBA", "Inter_OneStep");
        addConnection(6, QString::fromUtf8("Աբովյան"), "10.10.13.1", "cafe4", "SYSDBA", "Inter_OneStep");
        addConnection(7, QString::fromUtf8("Արմենիա"), "10.10.7.1", "cafe4", "SYSDBA", "Inter_OneStep");
        addConnection(8, QString::fromUtf8("Երևան Մոլլ"), "10.10.14.1", "cafe4", "SYSDBA", "Inter_OneStep");
        fStorages[2] = QString::fromUtf8("Բար");
        fStorages[3] = QString::fromUtf8("Խոհանոց");
        fStorages[4] = QString::fromUtf8("Սառնարան");
        fAsStorageMap[2][2] = "02";
        fAsStorageMap[2][3] = "04";
        fAsStorageMap[2][4] = "05";
        fAsStorageMap[3][2] = "031";
        fAsStorageMap[3][3] = "032";
        fAsStorageMap[3][4] = "033";
        fAsStorageMap[4][2] = "041";
        fAsStorageMap[4][3] = "042";
        fAsStorageMap[4][4] = "043";
        fAsStorageMap[6][2] = "061";
        fAsStorageMap[6][3] = "062";
        fAsStorageMap[6][4] = "063";
        fAsStorageMap[7][2] = "071";
        fAsStorageMap[8][2] = "081";
        fAsStorageMap[8][3] = "082";
        fAsStorageMap[8][4] = "083";
    }
}

bool requestGoodsGroups(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    Database db("QIBASE");
    if (!db.open("10.1.0.4", "c:\\fb\\maindb.fdb", "SYSDBA", "Inter_OneStep")) {
        err = "Connection to 10.1.0.4 failed with " + db.lastDbError();
        return false;
    }
    QMap<int, QJsonObject> goods;
    db.exec("select f.id, t.name as typename, f.name as foodname "
            "from food_names f "
            "left join food_groups t on t.id=f.group_id ");
    while (db.next()) {
        QJsonObject r;
        for (int i = 0; i < db.columnCount(); i++) {
            r[db.columnName(i).toUpper()] = db.value(i).toJsonValue();
        }
        goods[db.integer("id")] = r;
    }
    QString goodsArray;
    for (QMap<int, QJsonObject>::const_iterator it = goods.constBegin(); it != goods.constEnd(); it++) {
        if (!goodsArray.isEmpty()) {
            goodsArray += ",";
        }
        goodsArray += it.value()["f_id"].toString();
    }
    goodsArray = "[" + goodsArray + "]";
    jret["data"] = goodsArray;
    return true;
}

bool requestStore(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    int cafe = jreq["cafe"].toInt();
    int store = jreq["store"].toInt();
    QMap<int, QJsonObject> goods;
    QMap<QString, QString> con = fConnections[cafe];
    Database db("QIBASE");
    if (!db.open(con["host"], con["schema"], con["username"], con["password"])) {
        jret["errorCode"] = 1;
        err = "Connection to " + con["host"] + con["schema"] + " failed with " + db.lastDbError();
        return false;
    }
    db.exec("select * from sys_as_conn");
    QString mssql_conn, dbname;
    if (db.next()) {
        dbname = db.string("database");
        mssql_conn = QString("%1;DATABASE=%2").arg(db.string("conn_str"), db.string("database"));
    }
    db.exec("select f.id, t.name as typename, f.name as foodname "
            "from food_names f "
            "left join food_groups t on t.id=f.group_id "
            "order by f.name ");
    while (db.next()) {
        QJsonObject r;
        for (int i = 0; i < db.columnCount(); i++) {
            r[db.columnName(i).toUpper()] = db.value(i).toJsonValue();
        }
        goods[db.integer("id")] = r;
    }
    int month = jreq["month"].toInt() + 1;
    QList<int> years;
    years.append(2021);
    years.append(2022);
    years.append(2023);
    years.append(2024);
    years.append(2025);
    int year = jreq["year"].toInt();
    year = years.at(year);
    QDate d = QDate::currentDate();
    QDate d1 = QDate::fromString(QString("%1/%2/%3").arg(1, 2, 10, QChar('0')).arg(month, 2, 10, QChar('0')).arg(year),
                                 "dd/MM/yyyy");
    QDate d2 = QDate::fromString(QString("%1/%2/%3").arg(d1.daysInMonth(), 2, 10, QChar('0')).arg(month, 2, 10,
                                 QChar('0')).arg(year), "dd/MM/yyyy");
    jret["date_range"] = QString("%1 - %2").arg(d1.toString(), d2.toString());
    db[":date"] = d1.addDays(-1);
    db[":store_id"] = store;
    db[":action_id"] = 7;
    if (!db.exec("select sd.goods_id, sum(sd.qty) as qty, sum(sd.amount) as amount "
                 "from st_documents st, st_draft sd "
                 "where st.id=sd.doc_id "
                 "and st.doc_date = :date and st.store_input=:store_id "
                 "and st.action_id=:action_id "
                 "group by 1")) {
        jret["errorCode"] = 1;
        err = db.lastDbError();
        return false;
    }
    LogWriter::write(LogWriterLevel::verbose, "Last inventory data", db.lastQuery());
    while (db.next()) {
        QJsonObject j = goods[db.integer("goods_id")];
        j["QTY_TILL"] = db.doubleValue("qty");
        j["AMOUNT_TILL"] = db.doubleValue("amount");
        goods[db.integer("goods_id")] = j;
    }
    Database mssqldb("QODBC");
    LogWriter::write(LogWriterLevel::verbose, "MSSQL", mssql_conn);
    if (!mssqldb.open("10.1.0.4,1433", mssql_conn, "sa", "SaSa111")) {
        jret["errorCode"] = 1;
        err = "Connection to 10.1.0.4,1433 failed with " + mssqldb.lastDbError();
        return false;
    }
    mssqldb[":store"] = fAsStorageMap[cafe][store];
    mssqldb[":date1"] = d1;
    mssqldb[":date2"] = d2;
    if (!mssqldb.exec(
                QString("select cast(t.fMTCODE as integer) as food, m.fDBCR as sign, "
                        "sum(m.fQTY) as qty, sum(m.fCOSTSUMM) as amount "
                        "from %1.dbo.MTHI m, %1.dbo.MATERIALS t , %1.dbo.DOCUMENTS d "
                        "where m.fMTID=t.fMTID and d.fISN=m.fBASE and d.fDOCTYPE in (6, 7, 8, 17) "
                        "and d.fDATE between :date1 and :date2 and cast(m.fSTORAGE as integer)=cast(:store as integer)"
                        "and (t.fMTCODE NOT LIKE '1-%' and t.fMTCODE NOT LIKE '2-%') "
                        "group by cast(t.fMTCODE as integer), m.fDBCR "
                        "order by 1").arg(dbname))) {
        jret["errorCode"] = 1;
        err = mssqldb.lastDbError();
        return false;
    }
    QStringList foodNotInDb;
    int mssqlrowcount = 0;
    while (mssqldb.next()) {
        mssqlrowcount ++;
        if (!goods.contains(mssqldb.integer("food"))) {
            if (!foodNotInDb.contains(mssqldb.string("food"))) {
                foodNotInDb.append(mssqldb.string("food"));
            }
            continue;
        }
        QJsonObject j = goods[mssqldb.integer("food")];
        j["QTY_IN"] = j["QTY_IN"].toDouble() + (mssqldb.doubleValue("qty") * (mssqldb.integer("sign") == 0 ? -1 : 1));
        j["AMOUNT_IN"] = j["AMOUNT_IN"].toDouble() + (mssqldb.doubleValue("amount") * (mssqldb.integer("sign") == 0 ? -1 : 1));
        goods[mssqldb.integer("food")] = j;
    }
    LogWriter::write(LogWriterLevel::verbose, QString("mssql records: %1").arg(mssqlrowcount), mssqldb.lastQuery());
    if (foodNotInDb.count() > 0) {
        err = QString("Goods not in database: %1").arg(foodNotInDb.join(","));
        return false;
    }
    db[":store"] = store;
    db[":date1"] = d1;
    db[":date2"] = d2;
    if (!db.exec("select r.goods_id, sum(r.qty*d.qty) as qty, 0 as AMOUNT "
                 "from o_dishes d  "
                 "left join me_recipes r on d.dish_id=r.dish_id "
                 "left join o_order o on o.id=d.order_id "
                 "where o.date_cash between :date1 and :date2 "
                 "and (o.state_id=2 or o.state_id=1) and d.state_id=1 "
                 "and r.goods_id is not null "
                 "and d.store_id=:store "
                 "group by 1")) {
        jret["errorCode"] = 1;
        err = db.lastDbError();
        return false;
    }
    while (db.next()) {
        QJsonObject j = goods[db.integer("goods_id")];
        j["QTY_OUT"] = db.doubleValue("qty");
        j["AMOUNT_OUT"] = db.doubleValue("amount");
        goods[db.integer("goods_id")] = j;
    }
    //OUT2
    db[":store"] = store;
    db[":date1"] = d1;
    db[":date2"] = d2;
    if (!db.exec("select fn.id as goods_id, sum(sdd.qty) as qty,sum(sdd.amount) as amount "
                 "from st_documents sd "
                 "left join st_actions sa on sd.action_id=sa.id "
                 "left join st_storages s2 on s2.id=sd.store_output "
                 "left join st_draft sdd on sdd.doc_id=sd.id "
                 "left join food_names fn on fn.id=sdd.goods_id "
                 "where sd.doc_date between :date1 and :date2 and sa.id in (2) and s2.id=:store "
                 "group by 1 ")) {
        jret["errorCode"] = 1;
        err = db.lastDbError();
        return false;
    }
    while (db.next()) {
        QJsonObject j = goods[db.integer("goods_id")];
        j["QTY_OUT2"] = db.doubleValue("qty");
        j["AMOUNT_OUT2"] = db.doubleValue("amount");
        goods[db.integer("goods_id")] = j;
    }
    QString goodsArray;
    QJsonArray jgoods;
    for (QMap<int, QJsonObject>::iterator it = goods.begin(); it != goods.end(); it++) {
        if (it.value()["QTY_TILL"].isNull()) {
            it.value()["QTY_TILL"] = 0;
        }
        if (it.value()["QTY_IN"].isNull()) {
            it.value()["QTY_IN"] = 0;
        }
        if (it.value()["QTY_OUT"].isNull()) {
            it.value()["QTY_OUT"] = 0;
        }
        if (it.value()["QTY_OUT2"].isNull()) {
            it.value()["QTY_OUT2"] = 0;
        }
        if (it.value()["QTY_TILL"].toDouble() < 0.0001
                && it.value()["QTY_IN"].toDouble() < 0.0001
                && it.value()["QTY_OUT"].toDouble() < 0.0001
                && it.value()["QTY_OUT2"].toDouble() < 0.0001) {
            continue;
        }
        if (!goodsArray.isEmpty()) {
            goodsArray += ",";
        }
        jgoods.append(it.value());
        goodsArray += it.value()["ID"].toString();
    }
    goodsArray = "[" + goodsArray + "]";
    jret["data"] = goodsArray;
    jret["goods"] = jgoods;
    return true;
}

void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username,
                   const QString &password)
{
    QMap<QString, QString> c;
    c["name"] = name;
    c["host"] = host;
    c["schema"] = schema;
    c["username"] = username;
    c["password"] = password;
    fConnections[id] = c;
}
