#include "jzstore.h"
#include "database.h"
#include "jsonhandler.h"

QMap<int, QMap<QString, QString> > fConnections;
QMap<int, QString> fStorages;
QMap<int, QMap<int, QString> > fAsStorageMap;

void routes(QStringList &r)
{
    r.append("jzstore");
}

bool jzstore(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    init();
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    //VALIDATE
    QString message;
    if (!dataMap.contains("month")) {
        message += "The month is required.";
    } else {
        if (getData(indata, dataMap["month"]).toInt() < 0 || getData(indata, dataMap["month"]).toInt() > 11) {
            message += "Invalid month range.";
        }
    }
    if (!dataMap.contains("request")) {
        message += "The type of request is missing.";
    } else {
        QStringList requests;
        QString request = getData(indata, dataMap["request"]);
        requests.append("goodslist");
        requests.append("store");
        if (!requests.contains(request)) {
            message += QString("Invalid request type %1").arg(QString(getData(indata, dataMap["request"])));
        } else {
            if (request == "store") {
                if (!dataMap.contains("cafe")) {
                    message += "Cafe parameter missing.";
                } else {
                    if (!fConnections.contains(getData(indata, dataMap["cafe"]).toInt())) {
                        message += "Invalid cafe parameter";
                    }
                }
                if (!dataMap.contains("store")) {
                    message += "Store parameter missing.";
                } else {
                    if (!fStorages.contains(getData(indata, dataMap["store"]).toInt())) {
                        message += "Invalid store parameter";
                    }
                }
            }
        }
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        return rh.setDataValidationError(jh.toString());
    }
    //PROCESS
    QString request = getData(indata, dataMap["request"]);
    if (request == "goodslist") {
        return requestGoodsGroups(rh, indata, dataMap);
    } else if (request == "store") {
        return requestStore(rh, indata, dataMap);
    }
    return rh.setForbiddenError("Where are you going?");
}

void init() {
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

        fAsStorageMap[2][2] = "001";
        fAsStorageMap[2][3] = "002";
        fAsStorageMap[2][4] = "003";
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

bool requestGoodsGroups(RequestHandler &rh, const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db("QIBASE");
    if (!db.open("10.1.0.4", "c:\\fb\\maindb.fdb", "SYSDBA", "Inter_OneStep")) {
        return rh.setInternalServerError("QIBASE " + db.lastDbError());
    }
    QMap<int, JsonHandler> goods;
    db.exec("select f.id, t.name as typename, f.name as foodname "
            "from food_names f "
            "left join food_groups t on t.id=f.group_id ");
    while (db.next()) {
        JsonHandler r;
        for (int i = 0; i < db.columnCount(); i++) {
            r[db.columnName(i).toUpper()] = db.value(i);
        }
        goods[db.integer("id")] = r;
    }
    QString goodsArray;
    for (QMap<int, JsonHandler>::const_iterator it = goods.constBegin(); it != goods.constEnd(); it++) {
        if (!goodsArray.isEmpty()) {
            goodsArray += ",";
        }
        goodsArray += it.value().toString();
    }
    goodsArray = "[" + goodsArray + "]";
    jh["status"] = "ok";
    jh["data"] = goodsArray;
    return rh.setResponse(HTTP_OK, jh.toString());
}

bool requestStore(RequestHandler &rh, const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    int cafe = getData(data, dataMap["cafe"]).toInt();
    int store = getData(data, dataMap["store"]).toInt();
    JsonHandler jh;
    QMap<int, JsonHandler> goods;
    QMap<QString, QString> con = fConnections[cafe];
    Database db("QIBASE");
    if (!db.open(con["host"], con["schema"], con["username"], con["password"])) {
        return rh.setInternalServerError(db.lastDbError());
    }
    db.exec("select * from sys_as_conn");
    QString mssql_conn, dbname;
    if (db.next()) {
        dbname = db.string("database");
        mssql_conn = QString("%1;DATABASE=%2").arg(db.string("conn_str"), db.string("database"));
        qDebug() << "CONNECTION" << mssql_conn;
    }
    db.exec("select f.id, t.name as typename, f.name as foodname "
            "from food_names f "
            "left join food_groups t on t.id=f.group_id "
            "order by f.name ");
    while (db.next()) {
        JsonHandler r;
        for (int i = 0; i < db.columnCount(); i++) {
            r[db.columnName(i).toUpper()] = db.value(i);
        }
        goods[db.integer("id")] = r;
    }
    int month = getData(data, dataMap["month"]).toInt() + 1;
    QList<int> years;
    years.append(2021);
    years.append(2022);
    years.append(2023);
    years.append(2024);
    int year = getData(data, dataMap["year"]).toInt();
    year = years.at(year);
    QDate d = QDate::currentDate();
    QDate d1 = QDate::fromString(QString("%1/%2/%3").arg(1, 2, 10, QChar('0')).arg(month, 2, 10, QChar('0')).arg(year), "dd/MM/yyyy");
    QDate d2 = QDate::fromString(QString("%1/%2/%3").arg(d1.daysInMonth(), 2, 10, QChar('0')).arg(month, 2, 10, QChar('0')).arg(year), "dd/MM/yyyy");
    db[":date"] = d1.addDays(-1);
    db[":store_id"] = store;
    db[":action_id"] = 7;
    if (!db.exec("select sd.goods_id, sum(sd.qty) as qty, sum(sd.amount) as amount "
                                 "from st_documents st, st_draft sd "
                                 "where st.id=sd.doc_id "
                                 "and st.doc_date = :date and st.store_input=:store_id "
                                 "and st.action_id=:action_id "
                                 "group by 1")) {
        return rh.setInternalServerError(db.lastDbError());
    }
    while (db.next()) {
        JsonHandler &j = goods[db.integer("goods_id")];
        j["QTY_TILL"] = db.doubleValue("qty");
        j["AMOUNT_TILL"] = db.doubleValue("amount");
    }
    Database mssqldb("QODBC3");
    if (!mssqldb.open("10.1.0.4,1433", mssql_conn, "sa", "SaSa111")) {
        return rh.setInternalServerError(mssqldb.lastDbError());
    }
    mssqldb[":store"] = fAsStorageMap[cafe][store];
    mssqldb[":date1"] = d1;
    mssqldb[":date2"] = d2;
    if (!mssqldb.exec(QString("select cast(t.fMTCODE as integer) as food, m.fDBCR as sign, sum(m.fQTY) as qty, sum(m.fCOSTSUMM) as amount "
                "from %1.dbo.MTHI m, %1.dbo.MATERIALS t , %1.dbo.DOCUMENTS d "
                "where m.fMTID=t.fMTID and d.fISN=m.fBASE and d.fDOCTYPE in (6, 7, 8, 17) "
                "and d.fDATE between :date1 and :date2 and m.fSTORAGE=:store "
                "and (t.fMTCODE NOT LIKE '1-%' and t.fMTCODE NOT LIKE '2-%') "
                "group by cast(t.fMTCODE as integer), m.fDBCR "
                "order by 1").arg(dbname))) {
        return rh.setInternalServerError(mssqldb.lastDbError());
    }
    QStringList foodNotInDb;
    while (mssqldb.next()) {
        if (!goods.contains(mssqldb.integer("food"))) {
            if (!foodNotInDb.contains(mssqldb.string("food"))) {
                foodNotInDb.append(mssqldb.string("food"));
            }
            continue;
        }
        JsonHandler &j = goods[mssqldb.integer("food")];
        j["QTY_IN"] = j["QTY_IN"].toDouble() + (mssqldb.doubleValue("qty") * (mssqldb.integer("sign") == 0 ? -1 : 1));
        j["AMOUNT_IN"] = j["AMOUNT_IN"].toDouble() + (mssqldb.doubleValue("amount") * (mssqldb.integer("sign") == 0 ? -1 : 1));
    }
    if (foodNotInDb.count() > 0) {
        return rh.setInternalServerError(QString("Goods not in database: %1").arg(foodNotInDb.join(",")));
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
        return rh.setInternalServerError(db.lastDbError());
    }
    while (db.next()) {
        JsonHandler &j = goods[db.integer("goods_id")];
        j["QTY_OUT"] = db.doubleValue("qty");
        j["AMOUNT_OUT"] = db.doubleValue("amount");
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
        return rh.setInternalServerError(db.lastDbError());
    }
    while (db.next()) {
        JsonHandler &j = goods[db.integer("goods_id")];
        j["QTY_OUT2"] = db.doubleValue("qty");
        j["AMOUNT_OUT2"] = db.doubleValue("amount");
    }
    QString goodsArray;
    for (QMap<int, JsonHandler>::iterator it = goods.begin(); it != goods.end(); it++) {
        if (it.value()["QTY_TILL"].toDouble() < 0.0001
                && it.value()["QTY_IN"].toDouble() < 0.0001
                && it.value()["QTY_OUT"].toDouble() < 0.0001
                && it.value()["QTY_OUT2"].toDouble() < 0.0001) {
            continue;
        }
        if (!goodsArray.isEmpty()) {
            goodsArray += ",";
        }
        goodsArray += it.value().toString();
    }
    goodsArray = "[" + goodsArray + "]";
    jh["status"] = "ok";
    jh["data"] = goodsArray;
    return rh.setResponse(HTTP_OK, jh.toString());
}

void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username, const QString &password)
{
    QMap<QString, QString> c;
    c["name"] = name;
    c["host"] = host;
    c["schema"] = schema;
    c["username"] = username;
    c["password"] = password;
    fConnections[id] = c;
}
