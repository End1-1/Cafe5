#include "shoprequest.h"
#include "jsonhandler.h"
#include "databaseconnectionmanager.h"
#include "shopmanager.h"
#include "configini.h"
#include "storemanager.h"
#include "printtaxn.h"
#include "printreceiptgroup.h"
#include <QJsonDocument>
#include <QUuid>
#include <QJsonArray>
#include <QMutex>

QMutex fMutex;

ShopRequest::ShopRequest() :
    RequestHandler()
{

}

bool ShopRequest::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db;
    if (!DatabaseConnectionManager::openDatabase(ShopManager::databaseName(), db, jh)) {
        return setInternalServerError(jh.toString());
    }

    int hall = ConfigIni::value("shop/hall").toInt();
    int table = ConfigIni::value("shop/table").toInt();

    /* ORDER ID */
    QString prefix;
    int hallid;
    db.startTransaction();
    db[":f_id"] = hall;
    db.exec("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=:f_id for update");
    if (db.next()) {
        hallid = db.integerValue("f_counter");
        prefix = db.stringValue("f_prefix");
        db[":f_counter"] = db.integerValue("f_counter");
        db.update("h_halls", "f_id", hall);
    } else {
        setInternalServerError(db.lastDbError());
        db.rollback();
        return false;
    }
    db.commit();

    double amountTotal = 0;
    //Check for multiple quantity and separate one by one
    QJsonArray ja = fOrderJson["items"].toArray();
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        while (o["qty"].toDouble() > 1) {
            o["qty"] = o["qty"].toDouble() - 1;
            ja[i] = o;
            QJsonObject oc = o;
            oc["qty"] = 1;
            oc["row"] = ja.count();
            ja.append(oc);
        }
    }
    qDebug() << fOrderJson;
    fOrderJson["items"] = ja;
    qDebug() << fOrderJson;
    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        amountTotal += o["price"].toDouble() * o["qty"].toDouble();
    }
    if (fOrderJson["delivery"].toDouble() > 0) {
        amountTotal += fOrderJson["delivery"].toDouble();
    }

    int otherid = 0;
    double amountCash = fOrderJson["payment_type"].toInt() == 1 ? amountTotal : 0;
    double amountCard = fOrderJson["payment_type"].toInt() == 2 ? amountTotal : 0;
    double amountOther = fOrderJson["payment_type"].toInt() > 4 ? amountTotal : 0;
    double amountIDram = fOrderJson["payment_type"].toInt() == 6 ? amountTotal : 0;
    double amountTellCell = fOrderJson["payment_type"].toInt() == 7 ? amountTotal : 0;
    db.startTransaction();
    /* O_HEADER */
    QString uuid = db.uuid();
    db[":f_id"] = uuid;
    db[":f_hallid"] = hallid;
    db[":f_prefix"] = prefix;
    db[":f_state"] = 2;
    db[":f_hall"] = hall;
    db[":f_table"] = table;
    db[":f_dateopen"] = QDate::currentDate();
    db[":f_dateclose"] = QDate::currentDate();
    db[":f_datecash"] = QDate::currentDate();
    db[":f_timeopen"] = QTime::currentTime();
    db[":f_timeclose"] = QTime::currentTime();
    db[":f_staff"] = 1;
    db[":f_comment"] = tr("Delivery address") + ": "
            + fOrderJson["address"].toObject()["city"].toString()
            + ", " + fOrderJson["address"].toObject()["address"].toString()
            + ", " + tr("phone") + ": "
            + fOrderJson["address"].toObject()["phone"].toString();
    db[":f_print"] = 0;
    db[":f_amounttotal"] = amountTotal;
    db[":f_amountcash"] = amountCash;
    db[":f_amountcard"] = amountCard;
    db[":f_amountbank"] = 0;
    db[":f_amountother"] = amountOther;
    db[":f_amountservice"] = 0;
    db[":f_amountdiscount"] = 0;
    db[":f_servicefactor"] = 0;
    db[":f_discountfactor"] = 0;
    db[":f_creditcardid"] = 0;
    db[":f_otherid"] = 0;
    db[":f_shift"] = 1;
    db[":f_source"] = 2;
    db[":f_saletype"] = 1;
    db[":f_partner"] = 0;
    if (!db.insert("o_header")) {
        setInternalServerError(db.lastDbError());
        db.rollback();
        return false;
    }
    db[":f_id"] = uuid;
    db[":f_1"] = 0;
    db[":f_2"] = 0;
    db[":f_3"] = 0;
    db[":f_4"] = 0;
    db[":f_5"] = 0;
    if (!db.insert("o_header_flags")) {
        setInternalServerError(db.lastDbError());
        db.rollback();
        return false;
    }

    db[":f_id"] = uuid;
    db[":f_cash"] = amountCash;
    db[":f_card"] = amountCard;
    db[":f_prepaid"] = 0;
    db[":f_change"] = 0;
    db[":f_idram"] = amountIDram;
    db[":f_tellcell"] = amountTellCell;
    if (!db.insert("o_payment")) {
        setInternalServerError(db.lastDbError());
        db.rollback();
        return false;
    }

    /* PRINT TAX */
    QString err, rseq;
    if (ConfigIni::value("shop/taxport").toInt() > 0) {
        QString sn, firm, address, fiscal, hvhh, devnum, time;
        PrintTaxN pt(ConfigIni::value("shop/taxip"),
                     ConfigIni::value("shop/taxport").toInt(),
                     ConfigIni::value("shop/taxpass"),
                     ConfigIni::value("shop/taxextpos"),
                     ConfigIni::value("shop/taxcashier"),
                     ConfigIni::value("shop/taxpin"),
                     this);
        for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
            QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
            amountTotal += o["price"].toDouble() * o["qty"].toDouble();
            pt.addGoods(ConfigIni::value("shop/taxdep"), //dep
                        "6204", //adg
                        QString::number(StoreManager::codeOfSku(o["sku"].toString())), //goods id
                        StoreManager::nameOfSku(o["sku"].toString()), //name
                        o["price"].toDouble(), //price
                        o["qty"].toDouble(), //qty
                        0); //discount
        }
        if (fOrderJson["delivery"].toDouble() > 0) {
            amountTotal += fOrderJson["delivery"].toDouble();
            pt.addGoods(ConfigIni::value("shop/taxdep"), //dep
                        "6204", //adg
                        ConfigIni::value("shop/delivery"), //goods id
                        tr("Delivery"), //name
                        fOrderJson["delivery"].toDouble(), //price
                        1, //qty
                        0); //discount
        }
        QString jsonIn, jsonOut;
        int result = 0;
        result = pt.makeJsonAndPrint(amountCard, 0, jsonIn, jsonOut, err);
        if (result == pt_err_ok) {
            PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
            db[":f_id"] = uuid;
            db.exec("delete from o_tax where f_id=:f_id");
            db[":f_id"] = uuid;
            db[":f_dept"] = ConfigIni::value("shop/taxdep");
            db[":f_firmname"] = firm;
            db[":f_address"] = address;
            db[":f_devnum"] = devnum;
            db[":f_serial"] = sn;
            db[":f_fiscal"] = fiscal;
            db[":f_receiptnumber"] = rseq;
            db[":f_hvhh"] = hvhh;
            db[":f_fiscalmode"] = tr("(F)");
            db[":f_time"] = time;
            db.insert("o_tax");
        } else {
            jh["atention"] = err + "<br>" + jsonOut + "<br>" + jsonIn;
        }
    }

    /* A_HEADER of store */
    db[":f_id"] = ConfigIni::value("shop/store").toInt();
    db.exec("select f_outcounter + 1  as f_counter from c_storages where f_id=:f_id for update");
    if (!db.next()) {
        setInternalServerError("UNKNOWN STORE");
        db.rollback();
        return false;
    }
    int val = db.integerValue("f_counter");
    db[":f_id"] = ConfigIni::value("shop/store").toInt();
    db[":val"] = val;
    db.exec("update c_storages set f_outcounter=:val where f_id=:f_id and f_outcounter <:val");

    db[":f_id"] = uuid;
    db[":f_userid"] = val;
    db[":f_state"] = 1;
    db[":f_type"] = 2;
    db[":f_operator"] = 1;
    db[":f_date"] = QDate::currentDate();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = 0;
    db[":f_comment"] = "output of " + prefix + QString::number(hallid);
    db[":f_payment"] = 1;
    db[":f_paid"] = 0;
    db.insert("a_header");

    db[":f_id"] = uuid;
    db[":f_useraccept"] = 1;
    db[":f_userpass"] = 1;
    db[":f_invoice"] = "";
    db[":f_invoicedate"] = QVariant();
    db[":f_storein"] = 0;
    db[":f_storeout"] = ConfigIni::value("shop/store").toInt();
    db[":f_baseonsale"] = 1;
    db[":f_cashuuid"] = "";
    db[":f_complectation"] = 0;
    db[":f_complectationqty"] = 0;
    db.insert("a_header_store");

    /* A_STORE_DRAFT, O_GOODS */
    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        QString goodsid = db.uuid();

        db[":f_id"] = goodsid;
        db[":f_document"] = uuid;
        db[":f_store"] = ConfigIni::value("shop/store").toInt();
        db[":f_type"] = -1;
        db[":f_goods"] = StoreManager::codeOfSku(o["sku"].toString());
        db[":f_qty"] = o["qty"].toDouble();
        db[":f_price"] = 0;
        db[":f_total"] = 0;
        db[":f_reason"] = 4;
        db[":f_base"] = goodsid;
        db[":f_row"] = o["row"].toInt();
        db[":f_comment"] = "";
        db.insert("a_store_draft");

        db[":f_id"] = goodsid;
        db[":f_header"] = uuid;
        db[":f_body"] = goodsid;
        db[":f_store"] = ConfigIni::value("shop/store").toInt();
        db[":f_goods"] = StoreManager::codeOfSku(o["sku"].toString());
        db[":f_qty"] = o["qty"].toDouble();
        db[":f_price"] = o["price"].toDouble();
        db[":f_total"] = o["qty"].toDouble() * o["price"].toDouble();
        db[":f_tax"] = rseq;
        db[":f_sign"] = 1;
        db[":f_row"] = o["row"].toInt();
        db[":f_storerec"] = goodsid;
        db[":f_discountfactor"] = 0;
        db[":f_discountmode"] = 0;
        db[":f_discountamount"] = 0;
        db[":f_return"] = 0;
        db.insert("o_goods");
    }

    if (fOrderJson["delivery"].toDouble() > 0) {
        QString goodsid = db.uuid();
        db[":f_id"] = goodsid;
        db[":f_header"] = uuid;
        db[":f_body"] = goodsid;
        db[":f_store"] = ConfigIni::value("shop/store").toInt();
        db[":f_goods"] = ConfigIni::value("shop/delivery").toInt();
        db[":f_qty"] = 1;
        db[":f_price"] = fOrderJson["delivery"].toDouble();
        db[":f_total"] = fOrderJson["delivery"].toDouble();
        db[":f_tax"] = rseq;
        db[":f_sign"] = 1;
        db[":f_row"] = 100;
        db[":f_storerec"] = goodsid;
        db[":f_discountfactor"] = 0;
        db[":f_discountmode"] = 0;
        db[":f_discountamount"] = 0;
        db[":f_return"] = 0;
        db.insert("o_goods");
    }

    /* A_HEADER of cash */
    QString cashuuid = db.uuid();
    db[":f_id"] = cashuuid;
    db[":f_userid"] = val;
    db[":f_state"] = 1;
    db[":f_type"] = 5;
    db[":f_operator"] = 1;
    db[":f_date"] = QDate::currentDate();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = 0;
    db[":f_comment"] = "output of " + prefix + QString::number(hallid);
    db[":f_payment"] = 1;
    db[":f_paid"] = 0;
    db.insert("a_header");

    int cash = fOrderJson["payment_type"].toInt() == 1 ? ConfigIni::value("shop/cashcash").toInt() : ConfigIni::value("shop/cashcard").toInt();
    db[":f_id"] = cashuuid;
    db[":f_cashin"] = cash;
    db[":f_cashout"] = 0;
    db[":f_related"] = 0;
    db[":f_storedoc"] = "";
    db[":f_oheader"] = uuid;
    db.insert("a_header_cash");

    db[":f_id"] = cashuuid;
    db[":f_header"] = cashuuid;
    db[":f_cash"] = cash;
    db[":f_sign"] = 1;
    db[":f_remarks"] = "input of sale";
    db[":f_amount"] = amountTotal;
    db[":f_base"] = cashuuid;
    db[":f_row"] = 1;
    db.insert("e_cash");

    jh["status"] = "OK";
    jh["uuid"] = uuid;
    jh["order_id"] = QString("%1%2").arg(prefix).arg(hallid);
    jh["before_store_error"] = err;
    db.commit();

    checkQty(QString("%1%2").arg(prefix).arg(hallid), db);

    db.startTransaction();
    err = "";
    if (!writeOutput(uuid, err, db)) {
        db.rollback();
        jh["store"] = err;
    }else {
        jh["store"] = "ok";
    }

    db.commit();

    PrintReceiptGroup p;
    p.print2(uuid, db, ConfigIni::value("shop/printer"));
    return setResponse(HTTP_OK, jh.toString());
}

bool ShopRequest::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    switch (fContentType) {
    case ContentType::ApplilcationJson:
        return validateApplicationJson(data, dataMap);
    default:
        return validateMultipartFormData(data, dataMap);
    }
    return false;
}

bool ShopRequest::validateMultipartFormData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    if (!dataMap.contains("key")) {
        message += "Product key missing.";
    } else {
        QString key = getData(data, dataMap["key"]);
        if (key != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    }
    if (!dataMap.contains("order")) {
        message += "Order missing.";
    } else {
        QJsonParseError jerror;
        QJsonDocument jdoc = QJsonDocument::fromJson(getData(data, dataMap["order"]), &jerror);
        if (jerror.error == QJsonParseError::NoError) {
            fOrderJson = jdoc.object();
            if (fOrderJson.isEmpty()) {
                message += "Emtpy order";
            }
        } else {
            message += jerror.errorString() + "-----" + getData(data, dataMap["order"]);
        }
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        fHttpHeader.setResponseCode(HTTP_DATA_VALIDATION_ERROR);
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    }
    return message.isEmpty();
}

bool ShopRequest::validateApplicationJson(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(getData(data, dataMap["json"]), &jerror);
    QJsonObject jo = jdoc.object();
    if (jerror.error == QJsonParseError::NoError) {
        fOrderJson = jo["order"].toObject();
        if (fOrderJson.isEmpty()) {
            message += "Emtpy order";
        } else {
            if (fOrderJson["items"].toArray().isEmpty()) {
                message += "Empty order";
            }
        }
        if (jo["key"].toString() != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    } else {
        message += jerror.errorString() + "-----" + getData(data, dataMap["json"]);
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        fHttpHeader.setResponseCode(HTTP_DATA_VALIDATION_ERROR);
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    }
    return message.isEmpty();
}

bool ShopRequest::writeOutput(const QString &docId, QString &err, Database &db)
{
    double amount = 0;
    int storeOut = 0;
    int storeIn = 0;
    int reason = 0;
    int docType = 0;
    QString complectId;
    int complectCode = 0;
    double complectQty = 0;
    QDate date;
    db[":f_id"] = docId;
    db.exec("select h.f_type, h.f_date, hh.f_storein, hh.f_storeout, hh.f_complectation, hh.f_complectationqty"
               " from a_header h inner join a_header_store hh on hh.f_id=h.f_id where h.f_id=:f_id");
    if (db.next()) {
        date = db.dateValue("f_date");
        storeIn = db.integerValue("f_storein");
        storeOut = db.integerValue("f_storeout");
        docType = db.integerValue("f_type");
        complectCode = db.integerValue("f_complectation");
        complectQty = db.doubleValue("f_complectationqty");
    } else {
        err += tr("Invalid document id") + "<br>";
    }
    if (complectCode > 0) {
        db[":f_document"] = docId;
        db[":f_type"] = 1;
        db.exec("select f_id from a_store_draft where f_document=:f_document and f_type=:f_type");
        if (db.next()) {
            complectId = db.stringValue(0);
        }
    }

    QStringList recID;
    QStringList baseID;
    QStringList goodsID;
    QStringList goodsName;
    QList<double> qtyList;
    QList<double> priceList;
    QList<double> totalList;
    db[":f_document"] = docId;
    db.exec("select s.f_id, s.f_goods, s.f_store, s.f_qty, s.f_price, s.f_total, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, s.f_base, s.f_reason "
               "from a_store_draft s inner join c_goods g on g.f_id=s.f_goods "
               "where f_document=:f_document and f_type=-1");
    while (db.next()) {
        recID.append(db.stringValue("f_id"));
        baseID.append(db.stringValue("f_base"));
        goodsID.append(db.stringValue("f_goods"));
        goodsName.append(db.stringValue("f_name"));
        reason = db.integerValue("f_reason");
        qtyList.append(db.doubleValue("f_qty"));
        priceList.append(db.doubleValue("f_price"));
        totalList.append(db.doubleValue("f_total"));
    }

    QList<QMap<QString, QVariant> > storeData;
    db[":f_store"] = storeOut;
    db[":f_date"] = date;
    if (!db.exec(QString("select s.f_id, s.f_goods, sum(s.f_qty*s.f_type) as f_qty, s.f_price, s.f_total*s.f_type, "
                         "s.f_document, s.f_base, d.f_date "
                           "from a_store s "
                           "inner join a_header d on d.f_id=s.f_document "
                           "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
                           "group by s.f_base "
                           "having sum(s.f_qty*s.f_type)>0 "
                           "order by d.f_date "
                           "for update ").arg(goodsID.join(",")))) {
        err = db.lastDbError() + "<br>";
        return false;
    }
    while (db.next()) {
        QMap<QString, QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v[db.columnName(i)] = db.value(i);
        }
        storeData.append(v);
    }
    QList<QMap<QString, QVariant> > queries;
    for (int i = 0; i < goodsID.count(); i++) {
        double qty = qtyList.at(i);
        totalList[i] = 0;
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j)["f_goods"].toInt() == goodsID.at(i).toInt()) {
                if (storeData.at(j)["f_qty"].toDouble() > 0) {
                    if (storeData.at(j)["f_qty"].toDouble() >= qty) {
                        storeData[j]["f_qty"] = storeData.at(j)["f_qty"].toDouble() - qty;
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j)["f_price"];
                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j)["f_base"].toString();
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        amount += storeData.at(j)["f_price"].toDouble() * qty;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_price"].toDouble() * qty);
                        priceList[i] = totalList[i] / qtyList.at(i);
                        qty = 0;
                    } else {
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = storeData.at(j)["f_qty"].toDouble();
                        newrec[":f_price"] = storeData.at(j)["f_price"].toDouble();
                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * storeData.at(j)["f_qty"].toDouble();
                        newrec[":f_base"] = storeData.at(j)["f_base"];
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble());
                        priceList[i] = totalList[i] / qtyList.at(i);
                        amount += storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble();
                        qty -= storeData.at(j)["f_qty"].toDouble();
                        storeData[j]["f_qty"] = 0.0;
                    }
                }
            }
            if (qty < 0.00001) {
                break;
            }
        }
        if (qty > 0.00001) {
            if (err.isEmpty()) {
                err += tr("Not enough materials in the store") + "<br>";
            }
            err += QString("%1 - %2").arg(goodsName.at(i)).arg(qty) + "<br>";
        }
    }
    if (err.isEmpty()) {
        QStringList outId;
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.begin(); it != queries.end(); it++) {
            QString newId = QUuid::createUuid().toString().replace("{", "").replace("}", "");
            outId << newId;
            db.setBindValues(*it);
            db[":f_id"] = newId;
            db.insert("a_store");
        }

        db[":f_document"] = docId;
        db.exec("select f_type, sum(f_total) as f_total from a_store where f_document=:f_document group by 1");
        if (db.next()) {
            db[":f_amount"] = db.doubleValue("f_total");
            db.update("a_header", "f_id", docId);
        }

        if (storeIn > 0) {
            switch (docType) {
            case 3:
                foreach (QString recid, outId) {
                    db[":f_id"] = recid;
                    db.exec("select * from a_store where f_id=:f_id");
                    db.next();
                    db.setBindValues(db.getBindValues());
                    QString mid = QUuid::createUuid().toString().replace("{", "").replace("}", "");
                    db[":f_base"] = mid;
                    db[":f_basedoc"] = db[":f_id"];
                    db[":f_id"] = mid;
                    db[":f_document"] = docId;
                    db[":f_type"] = 1;
                    db[":f_store"] = storeIn;
                    if (!db.insert("a_store")) {
                        err = db.lastDbError();
                        return false;
                    }
                }
                break;
            case 6:
                db[":f_header"] = docId;
                db.exec("select sum(f_amount) from a_complectation_additions where f_header=:f_header");
                db.next();
                amount += db.doubleValue(0);
                QString id = QUuid::createUuid().toString().replace("{", "").replace("}", "");
                db[":f_id"] = id;
                db[":f_document"] = docId;
                db[":f_store"] = storeIn;
                db[":f_type"] = 1;
                db[":f_goods"] = complectCode;
                db[":f_qty"] = complectQty;
                db[":f_price"] = amount / complectQty;
                db[":f_total"] = amount;
                db[":f_base"] = outId.at(0);
                db[":f_basedoc"] = docId;
                db[":f_reason"] = reason;
                db[":f_draft"] = complectId;
                if (!db.insert("a_store")) {
                    err = db.lastDbError();
                    return false;
                }
                break;
            }
        }
        for (int i = 0; i < recID.count(); i++) {
            db[":f_price"] = priceList.at(i);
            db[":f_total"] = totalList.at(i);
            db.update("a_store_draft", "f_id", recID.at(i));
            if (!baseID.at(i).isEmpty()) {
                db[":f_price"] = priceList.at(i);
                db[":f_total"] = totalList.at(i);
                db.update("a_store_draft", "f_id", baseID.at(i));
            }
        }
        if (docType == 6) {
            db[":f_type"] = 1;
            db[":f_document"] = docId;
            db[":f_price"] = amount / complectQty;
            db[":f_total"] = amount;
            db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_type=:f_type and f_document=:f_document");
        }
    }

    return err.isEmpty();
}

bool ShopRequest::writeMovement(const QString &orderid, int goods, double qty, int srcStore, Database &db)
{
    db.startTransaction();
    db[":f_id"] = srcStore;
    db.exec("select f_name from c_storages where f_id=:f_id ");
    QString storename = "unknown";
    if (db.next()) {
        storename = db.stringValue("f_name");
    }
    QString id = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    db[":f_id"] = id;
    db[":f_userid"] = 1;
    db[":f_state"] = 2;
    db[":f_type"] = 3;
    db[":f_operator"] = 1;
    db[":f_date"] = QDate::currentDate();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = 0;
    db[":f_comment"] = tr("AUTOMATICALY (%1, %2)").arg(orderid, storename);
    db[":f_payment"] = 0;
    db[":f_paid"] = 0;
    if (!db.insert("a_header")) {
        db.rollback();
        return false;
    }

    db[":f_id"] = id;
    db[":f_useraccept"] = 0;
    db[":f_userpass"] = 0;
    db[":f_invoice"] = QVariant();
    db[":f_invoicedate"] = QVariant();
    db[":f_storein"] = ConfigIni::value("shop/store").toInt();
    db[":f_storeout"] = srcStore;
    db[":f_baseonsale"] = 0;
    db[":f_cashuuid"] = "";
    db[":f_complectation"] = 0;
    db[":f_complectationqty"] = 0;
    if (!db.insert("a_header_store")) {
        db.rollback();
        return false;
    }

    QString sid1 = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    db[":f_id"] = sid1;
    db[":f_document"] = id;
    db[":f_store"] = ConfigIni::value("shop/store").toInt();;
    db[":f_type"] = 1;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_price"] = 0;
    db[":f_total"] = 0;
    db[":f_reason"] = 2;
    db[":f_base"] = sid1;
    db[":f_row"] = 1;
    db[":f_comment"] = "";
    if (!db.insert("a_store_draft")) {
        db.rollback();
        return false;
    }

    QString sid2 = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    db[":f_id"] = sid2;
    db[":f_document"] = id;
    db[":f_store"] = srcStore;
    db[":f_type"] = -1;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_price"] = 0;
    db[":f_total"] = 0;
    db[":f_reason"] = 2;
    db[":f_base"] = sid2;
    db[":f_row"] = 1;
    db[":f_comment"] = "";
    if (!db.insert("a_store_draft")) {
        db.rollback();
        return false;
    }

    db.commit();
    db.startTransaction();
    QString err;
    if (!writeOutput(id, err, db)) {
        emit sendData(0, fSession, err, QVariant());
        db.rollback();
        return false;
    }

    db[":f_state"] = 1;
    db.update("a_header", "f_id", id);
    db.commit();
    return true;
}

void ShopRequest::checkQty(const QString &orderid, Database &db)
{
    JsonHandler jh;
    Database dbsys;
    if (!DatabaseConnectionManager::openDatabase(dbsys, jh)) {
        return;
    }
    QMap<QString ,double> qty;
    QStringList goods;
    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        qty[o["sku"].toString()] = qty[o["sku"].toString()] + o["qty"].toDouble();
        goods.append(o["sku"].toString());
    }
    QMap<int, QMap<QString, double> > out;
    StoreManager::queryQty(goods, out);
    QString storeorder = ConfigIni::value("shop/storeorder");
    if (out.contains(ConfigIni::value("shop/store").toInt())) {
        QMap<QString, double> store = out[ConfigIni::value("shop/store").toInt()];
        for (QMap<QString, double>::iterator iq = qty.begin(); iq != qty.end(); iq++) {
            if (!store.contains(iq.key())) {
                continue;
            }
            if (store[iq.key()] >= iq.value()) {
                qty[iq.key()] = 0;
            } else {
                iq.value() -= store[iq.key()];
            }
        }
    }
    QMap<int, int> usersStoreMap;
    dbsys.exec("select * from users_store");
    while (dbsys.next()) {
        usersStoreMap[dbsys.integerValue("fid")] = dbsys.integerValue("fuser");
    }
    for (const QString &so: storeorder.split(",", QString::SkipEmptyParts)) {
        if (!out.contains(so.toInt())) {
            continue;
        }
        QMap<QString, double> store = out[so.toInt()];
        for (QMap<QString, double>::iterator iq = qty.begin(); iq != qty.end(); iq++) {
            if (iq.value() < 0.0001) {
                continue;
            }
            if (!store.contains(iq.key())) {
                continue;
            }
            if (store[iq.key()] >= iq.value()) {
                writeMovement(orderid, StoreManager::codeOfSku(iq.key()), iq.value(), so.toInt(), db);

                dbsys[":fdateserver"] = QDateTime::currentDateTime();
                dbsys[":fstate"] = 1;
                dbsys[":fsender"] = ConfigIni::value("shop/store").toInt();
                dbsys[":freceiver"] = usersStoreMap[so.toInt()];
                dbsys[":fmessage"] = QString("%1 - %2 (%4) : %3").arg(iq.key()).arg(iq.value()).arg(tr("Moved to online store"), orderid);
                dbsys.insert("users_chat");
                qty[iq.key()] = 0;
            } else {
                writeMovement(orderid, StoreManager::codeOfSku(iq.key()), store[iq.key()], so.toInt(), db);

                dbsys[":fdateserver"] = QDateTime::currentDateTime();
                dbsys[":fstate"] = 1;
                dbsys[":fsender"] = ConfigIni::value("shop/store").toInt();
                dbsys[":freceiver"] = usersStoreMap[so.toInt()];
                dbsys[":fmessage"] = QString("%1 - %2 (%4): %3").arg(iq.key()).arg(store[iq.key()]).arg(tr("Moved to online store"), orderid);
                dbsys.insert("users_chat");
                iq.value() -= store[iq.key()];
            }
        }
    }
}

