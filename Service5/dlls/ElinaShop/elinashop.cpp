#include "elinashop.h"
#include "jsonhandler.h"
#include "printtaxn.h"
#include "storemanager.h"
#include "shopmanager.h"
#include "printreceiptgroup.h"
#include "logwriter.h"
#include "commandline.h"
#include "storemovement.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QApplication>

void routes(QStringList &r)
{
    r.append("storerequest");
    r.append("shoprequest");
    r.append("printtax");
}

//bool shoprequest_old(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
//{
//    //VALIDATION
//    CommandLine cl;
//    QString path;
//    cl.value("path", path);
//    QString configFile = path + "/handlers/shop.ini";

//    StoreManager::init(configFile);
//    ShopManager::init(configFile);
//    RequestHandler rh(outdata);
//    rh.fContentType = contentType;
//    QJsonObject fOrderJson;
//    switch (contentType) {
//    case ContentType::ApplilcationJson:
//        if (!validateApplicationJson(rh, fOrderJson, indata, dataMap)) {
//            return false;
//        }
//        break;
//    default:
//        if (!validateMultipartFormData(rh, fOrderJson, indata, dataMap)) {
//            return false;
//        }
//        break;
//    }
//    //PROCESS
//    JsonHandler jh;
//    Database db;
//    if (!db.open(configFile)) {
//        LogWriter::write(LogWriterLevel::errors, "", QString("ElinaShop::shoprequest").arg(db.lastDbError()));
//        jh["message"] = "Cannot connect to database";
//        return rh.setInternalServerError(jh.toString());
//    }
//    QSettings s(configFile, QSettings::IniFormat);
//    int hall = s.value("shop/hall").toInt();
//    int table = s.value("shop/table").toInt();

//    /* ORDER ID */
//    QString prefix;
//    int hallid;
//    db.startTransaction();
//    db[":f_id"] = hall;
//    db.exec("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=:f_id for update");
//    if (db.next()) {
//        hallid = db.integer("f_counter");
//        prefix = db.string("f_prefix");
//        db[":f_counter"] = db.integer("f_counter");
//        db.update("h_halls", "f_id", hall);
//    } else {
//        rh.setInternalServerError(db.lastDbError());
//        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::shoprequest. " + db.lastDbError());
//        db.rollback();
//        return false;
//    }
//    db.commit();

//    double amountTotal = 0;
//    //Check for multiple quantity and separate one by one
//    QJsonArray ja = fOrderJson["items"].toArray();
//    for (int i = 0; i < ja.count(); i++) {
//        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
//        while (o["qty"].toDouble() > 1) {
//            o["qty"] = o["qty"].toDouble() - 1;
//            ja[i] = o;
//            QJsonObject oc = o;
//            oc["qty"] = 1;
//            oc["row"] = ja.count();
//            ja.append(oc);
//        }
//    }
//    fOrderJson["items"] = ja;
//    qDebug() << fOrderJson;
//    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
//        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
//        amountTotal += o["price"].toDouble() * o["qty"].toDouble();
//    }
//    if (fOrderJson["delivery"].toDouble() > 0) {
//        amountTotal += fOrderJson["delivery"].toDouble();
//    }

//    int otherid = 0;
//    double amountCash = fOrderJson["payment_type"].toInt() == 1 ? amountTotal : 0;
//    double amountCard = fOrderJson["payment_type"].toInt() == 2 ? amountTotal : 0;
//    double amountOther = fOrderJson["payment_type"].toInt() > 4 ? amountTotal : 0;
//    double amountIDram = fOrderJson["payment_type"].toInt() == 6 ? amountTotal : 0;
//    double amountTellCell = fOrderJson["payment_type"].toInt() == 7 ? amountTotal : 0;
//    db.startTransaction();
//    /* O_draft_sale */
//    QString uuid = db.uuid();
//    db[":f_id"] = uuid;
//    db[":f_hallid"] = hallid;
//    db[":f_prefix"] = prefix;
//    db[":f_state"] = 2;
//    db[":f_hall"] = hall;
//    db[":f_table"] = table;
//    db[":f_dateopen"] = QDate::currentDate();
//    db[":f_dateclose"] = QDate::currentDate();
//    db[":f_datecash"] = QDate::currentDate();
//    db[":f_timeopen"] = QTime::currentTime();
//    db[":f_timeclose"] = QTime::currentTime();
//    db[":f_staff"] = 1;
//    db[":f_comment"] = QObject::tr("Delivery address") + ": "
//            + fOrderJson["address"].toObject()["city"].toString()
//            + ", " + fOrderJson["address"].toObject()["address"].toString()
//            + ", " + QObject::tr("phone") + ": "
//            + fOrderJson["address"].toObject()["phone"].toString();
//    db[":f_print"] = 0;
//    db[":f_amounttotal"] = amountTotal;
//    db[":f_amountcash"] = amountCash;
//    db[":f_amountcard"] = amountCard;
//    db[":f_amountbank"] = 0;
//    db[":f_amountother"] = amountOther;
//    db[":f_amountservice"] = 0;
//    db[":f_amountdiscount"] = 0;
//    db[":f_servicefactor"] = 0;
//    db[":f_discountfactor"] = 0;
//    db[":f_creditcardid"] = 0;
//    db[":f_otherid"] = 0;
//    db[":f_shift"] = 1;
//    db[":f_source"] = 2;
//    db[":f_saletype"] = 1;
//    db[":f_partner"] = 0;
//    if (!db.insert("o_header")) {
//        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::requestshop." + db.lastDbError());
//        rh.setInternalServerError(db.lastDbError());
//        db.rollback();
//        return false;
//    }
//    db[":f_id"] = uuid;
//    db[":f_1"] = 0;
//    db[":f_2"] = 0;
//    db[":f_3"] = 0;
//    db[":f_4"] = 0;
//    db[":f_5"] = 0;
//    if (!db.insert("o_header_flags")) {
//        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::requestshop." + db.lastDbError());
//        rh.setInternalServerError(db.lastDbError());
//        db.rollback();
//        return false;
//    }
//    /* o_header_options */
//    db[":f_id"] = uuid;
//    db.insert("o_header_options");

//    db[":f_id"] = uuid;
//    db[":f_cash"] = amountCash;
//    db[":f_card"] = amountCard;
//    db[":f_prepaid"] = 0;
//    db[":f_change"] = 0;
//    db[":f_idram"] = amountIDram;
//    db[":f_tellcell"] = amountTellCell;
//    if (!db.insert("o_payment")) {
//        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::requestshop." + db.lastDbError());
//        rh.setInternalServerError(db.lastDbError());
//        db.rollback();
//        return false;
//    }

//    /* PRINT TAX */
//    QString err, rseq;
//    if (s.value("shop/taxport").toInt() > 0) {
//        QString sn, firm, address, fiscal, hvhh, devnum, time;
//        PrintTaxN pt(s.value("shop/taxip").toString(),
//                     s.value("shop/taxport").toInt(),
//                     s.value("shop/taxpass").toString(),
//                     s.value("shop/taxextpos").toString(),
//                     s.value("shop/taxcashier").toString(),
//                     s.value("shop/taxpin").toString(),
//                     nullptr);
//        for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
//            QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
//            amountTotal += o["price"].toDouble() * o["qty"].toDouble();
//            pt.addGoods(s.value("shop/taxdep").toString(), //dep
//                        "6204", //adg
//                        QString::number(StoreManager::codeOfSku(o["sku"].toString())), //goods id
//                        StoreManager::nameOfSku(o["sku"].toString()), //name
//                        o["price"].toDouble(), //price
//                        o["qty"].toDouble(), //qty
//                        0); //discount
//        }
//        if (fOrderJson["delivery"].toDouble() > 0) {
//            amountTotal += fOrderJson["delivery"].toDouble();
//            pt.addGoods(s.value("shop/taxdep").toString(), //dep
//                        "6204", //adg
//                        s.value("shop/delivery").toString(), //goods id
//                        QObject::tr("Delivery"), //name
//                        fOrderJson["delivery"].toDouble(), //price
//                        1, //qty
//                        0); //discount
//        }
//        QString jsonIn, jsonOut;
//        int result = 0;
//        result = pt.makeJsonAndPrint(amountCard, 0, jsonIn, jsonOut, err);
//        if (result == pt_err_ok) {
//            PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
//            db[":f_id"] = uuid;
//            db.exec("delete from o_tax where f_id=:f_id");
//            db[":f_id"] = uuid;
//            db[":f_dept"] = s.value("shop/taxdep");
//            db[":f_firmname"] = firm;
//            db[":f_address"] = address;
//            db[":f_devnum"] = devnum;
//            db[":f_serial"] = sn;
//            db[":f_fiscal"] = fiscal;
//            db[":f_receiptnumber"] = rseq;
//            db[":f_hvhh"] = hvhh;
//            db[":f_fiscalmode"] = QObject::tr("(F)");
//            db[":f_time"] = time;
//            db.insert("o_tax");
//        } else {
//            LogWriter::write(LogWriterLevel::warning, "", "ElinaShop::requestshop, taxprint." + err);
//            jh["atention"] = err + "<br>" + jsonOut + "<br>" + jsonIn;
//        }
//    }

//    /* A_HEADER of store */
//    db[":f_id"] = s.value("shop/store").toInt();
//    db.exec("select f_outcounter + 1  as f_counter from c_storages where f_id=:f_id for update");
//    if (!db.next()) {
//        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::requestshop. Unknown store");
//        rh.setInternalServerError("UNKNOWN STORE");
//        db.rollback();
//        return false;
//    }
//    int val = db.integer("f_counter");
//    db[":f_id"] = s.value("shop/store").toInt();
//    db[":val"] = val;
//    db.exec("update c_storages set f_outcounter=:val where f_id=:f_id and f_outcounter <:val");

//    db[":f_id"] = uuid;
//    db[":f_userid"] = val;
//    db[":f_state"] = 1;
//    db[":f_type"] = 2;
//    db[":f_operator"] = 1;
//    db[":f_date"] = QDate::currentDate();
//    db[":f_createdate"] = QDate::currentDate();
//    db[":f_createtime"] = QTime::currentTime();
//    db[":f_partner"] = 0;
//    db[":f_amount"] = 0;
//    db[":f_comment"] = "output of " + prefix + QString::number(hallid);
//    db[":f_payment"] = 1;
//    db[":f_paid"] = 0;
//    db.insert("a_header");

//    db[":f_id"] = uuid;
//    db[":f_useraccept"] = 1;
//    db[":f_userpass"] = 1;
//    db[":f_invoice"] = "";
//    db[":f_invoicedate"] = QVariant();
//    db[":f_storein"] = 0;
//    db[":f_storeout"] = s.value("shop/store").toInt();
//    db[":f_baseonsale"] = 1;
//    db[":f_cashuuid"] = "";
//    db[":f_complectation"] = 0;
//    db[":f_complectationqty"] = 0;
//    db.insert("a_header_store");

//    /* A_STORE_DRAFT, O_GOODS */
//    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
//        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
//        QString goodsid = db.uuid();

//        db[":f_id"] = goodsid;
//        db[":f_document"] = uuid;
//        db[":f_store"] = s.value("shop/store").toInt();
//        db[":f_type"] = -1;
//        db[":f_goods"] = StoreManager::codeOfSku(o["sku"].toString());
//        db[":f_qty"] = o["qty"].toDouble();
//        db[":f_price"] = 0;
//        db[":f_total"] = 0;
//        db[":f_reason"] = 4;
//        db[":f_base"] = goodsid;
//        db[":f_row"] = o["row"].toInt() + 1;
//        db[":f_comment"] = "";
//        db.insert("a_store_draft");

//        db[":f_id"] = goodsid;
//        db[":f_header"] = uuid;
//        db[":f_body"] = goodsid;
//        db[":f_store"] = s.value("shop/store").toInt();
//        db[":f_goods"] = StoreManager::codeOfSku(o["sku"].toString());
//        db[":f_qty"] = o["qty"].toDouble();
//        db[":f_price"] = o["price"].toDouble();
//        db[":f_total"] = o["qty"].toDouble() * o["price"].toDouble();
//        db[":f_tax"] = rseq;
//        db[":f_sign"] = 1;
//        db[":f_row"] = o["row"].toInt();
//        db[":f_storerec"] = goodsid;
//        db[":f_discountfactor"] = 0;
//        db[":f_discountmode"] = 0;
//        db[":f_discountamount"] = 0;
//        db[":f_return"] = 0;
//        db.insert("o_goods");
//    }

//    if (fOrderJson["delivery"].toDouble() > 0) {
//        QString goodsid = db.uuid();
//        db[":f_id"] = goodsid;
//        db[":f_header"] = uuid;
//        db[":f_body"] = goodsid;
//        db[":f_store"] = s.value("shop/store").toInt();
//        db[":f_goods"] = s.value("shop/delivery").toInt();
//        db[":f_qty"] = 1;
//        db[":f_price"] = fOrderJson["delivery"].toDouble();
//        db[":f_total"] = fOrderJson["delivery"].toDouble();
//        db[":f_tax"] = rseq;
//        db[":f_sign"] = 1;
//        db[":f_row"] = 100;
//        db[":f_storerec"] = goodsid;
//        db[":f_discountfactor"] = 0;
//        db[":f_discountmode"] = 0;
//        db[":f_discountamount"] = 0;
//        db[":f_return"] = 0;
//        db.insert("o_goods");
//    }

//    /* A_HEADER of cash */
//    QString cashuuid = db.uuid();
//    db[":f_id"] = cashuuid;
//    db[":f_userid"] = val;
//    db[":f_state"] = 1;
//    db[":f_type"] = 5;
//    db[":f_operator"] = 1;
//    db[":f_date"] = QDate::currentDate();
//    db[":f_createdate"] = QDate::currentDate();
//    db[":f_createtime"] = QTime::currentTime();
//    db[":f_partner"] = 0;
//    db[":f_amount"] = 0;
//    db[":f_comment"] = "output of " + prefix + QString::number(hallid);
//    db[":f_payment"] = 1;
//    db[":f_paid"] = 0;
//    db.insert("a_header");

//    int cash = fOrderJson["payment_type"].toInt() == 1 ? s.value("shop/cashcash").toInt() : s.value("shop/cashcard").toInt();
//    db[":f_id"] = cashuuid;
//    db[":f_cashin"] = cash;
//    db[":f_cashout"] = 0;
//    db[":f_related"] = 0;
//    db[":f_storedoc"] = "";
//    db[":f_oheader"] = uuid;
//    db.insert("a_header_cash");

//    db[":f_id"] = cashuuid;
//    db[":f_header"] = cashuuid;
//    db[":f_cash"] = cash;
//    db[":f_sign"] = 1;
//    db[":f_remarks"] = "input of sale";
//    db[":f_amount"] = amountTotal;
//    db[":f_base"] = cashuuid;
//    db[":f_row"] = 1;
//    db.insert("e_cash");

//    jh["status"] = "OK";
//    jh["uuid"] = uuid;
//    jh["order_id"] = QString("%1%2").arg(prefix).arg(hallid);
//    jh["before_store_error"] = err;
//    db.commit();

//    checkQty(fOrderJson, QString("%1%2").arg(prefix).arg(hallid), db);

//    db.startTransaction();
//    err = "";
//    if (!writeOutput(uuid, err, db)) {
//        db.rollback();
//        jh["store"] = err;
//        LogWriter::write(LogWriterLevel::errors, "", err);
//    }else {
//        jh["store"] = "ok";
//    }

//    db.commit();

//    PrintReceiptGroup p;
//    p.print2(uuid, db, s.value("shop/printer").toString());
//    return rh.setResponse(HTTP_OK, jh.toString());
//}

bool validateMultipartFormData(RequestHandler &rh, QJsonObject &ord, const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
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
            ord = jdoc.object();
            if (ord.isEmpty()) {
                message += "Emtpy order";
            }
        } else {
            message += jerror.errorString() + "-----" + getData(data, dataMap["order"]);
        }
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        return rh.setDataValidationError(jh.toString());
    }
    return true;
}


bool validateApplicationJson(RequestHandler &rh, QJsonObject &ord, const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString message;
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(getData(data, dataMap["json"]), &jerror);
    QJsonObject jo = jdoc.object();
    if (jerror.error == QJsonParseError::NoError) {
        ord = jo["order"].toObject();
        if (ord.isEmpty()) {
            message += "Emtpy order";
        } else {
            if (ord["items"].toArray().isEmpty()) {
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
        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::validatejson." + jh.toString());
        return rh.setDataValidationError(jh.toString());
    }
    return true;
}

bool writeMovement(const QString &orderid, int goods, double qty, int srcStore, Database &db)
{
    CommandLine cl;
    QString path;
    cl.value("path", path);
    QString configFile = path + "/handlers/shop.ini";
    QSettings s(configFile, QSettings::IniFormat);

    db.startTransaction();
    db[":f_id"] = srcStore;
    db.exec("select f_name from c_storages where f_id=:f_id ");
    QString storename = "unknown";
    if (db.next()) {
        storename = db.string("f_name");
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
    db[":f_comment"] = QObject::tr("AUTOMATICALY (%1, %2)").arg(orderid, storename);
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
    db[":f_storein"] = s.value("shop/store").toInt();
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
    db[":f_store"] = s.value("shop/store").toInt();;
    db[":f_type"] = 1;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_price"] = 0;
    db[":f_total"] = 0;
    db[":f_reason"] = 2;
    db[":f_base"] = "";
    db[":f_row"] = 0;
    db[":f_comment"] = storename;
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
    db[":f_base"] = sid1;
    db[":f_row"] = 0;
    db[":f_comment"] = storename;
    if (!db.insert("a_store_draft")) {
        db.rollback();
        return false;
    }

    db.commit();
    db.startTransaction();
    QString err;
    if (!writeOutput(id, err, db)) {
        db.rollback();
        return false;
    }

    db[":f_state"] = 1;
    db.update("a_header", "f_id", id);
    db.commit();
    return true;
}

//bool writeOutput(const QString &docId, QString &err, Database &db)
//{
//    double amount = 0;
//    int storeOut = 0;
//    int storeIn = 0;
//    int reason = 0;
//    int docType = 0;
//    QString complectId;
//    int complectCode = 0;
//    double complectQty = 0;
//    QDate date;
//    db[":f_id"] = docId;
//    db.exec("select h.f_type, h.f_date, hh.f_storein, hh.f_storeout, hh.f_complectation, hh.f_complectationqty"
//               " from a_header h inner join a_header_store hh on hh.f_id=h.f_id where h.f_id=:f_id");
//    if (db.next()) {
//        date = db.date("f_date");
//        storeIn = db.integer("f_storein");
//        storeOut = db.integer("f_storeout");
//        docType = db.integer("f_type");
//        complectCode = db.integer("f_complectation");
//        complectQty = db.doubleValue("f_complectationqty");
//    } else {
//        err += QObject::tr("Invalid document id") + "<br>";
//    }
//    if (complectCode > 0) {
//        db[":f_document"] = docId;
//        db[":f_type"] = 1;
//        db.exec("select f_id from a_store_draft where f_document=:f_document and f_type=:f_type");
//        if (db.next()) {
//            complectId = db.string(0);
//        }
//    }

//    QStringList recID;
//    QStringList baseID;
//    QStringList goodsID;
//    QStringList goodsName;
//    QStringList comments;
//    QList<double> qtyList;
//    QList<double> priceList;
//    QList<double> totalList;
//    db[":f_document"] = docId;
//    db.exec("select s.f_id, s.f_goods, s.f_store, s.f_qty, s.f_price, s.f_total, "
//            "concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, "
//            "s.f_base, s.f_reason, s.f_comment "
//               "from a_store_draft s inner join c_goods g on g.f_id=s.f_goods "
//               "where f_document=:f_document and f_type=-1");
//    while (db.next()) {
//        recID.append(db.string("f_id"));
//        baseID.append(db.string("f_base"));
//        goodsID.append(db.string("f_goods"));
//        goodsName.append(db.string("f_name"));
//        comments.append(db.string("f_comment"));
//        reason = db.integer("f_reason");
//        qtyList.append(db.doubleValue("f_qty"));
//        priceList.append(db.doubleValue("f_price"));
//        totalList.append(db.doubleValue("f_total"));
//    }

//    QList<QMap<QString, QVariant> > storeData;
//    db[":f_store"] = storeOut;
//    db[":f_date"] = date;
//    if (!db.exec(QString("select s.f_id, s.f_goods, sum(s.f_qty*s.f_type) as f_qty, s.f_price, s.f_total*s.f_type, "
//                         "s.f_document, s.f_base, d.f_date "
//                           "from a_store s "
//                           "inner join a_header d on d.f_id=s.f_document "
//                           "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
//                           "group by s.f_base "
//                           "having sum(s.f_qty*s.f_type)>0 "
//                           "order by d.f_date "
//                           "for update ").arg(goodsID.join(",")))) {
//        err = db.lastDbError() + "<br>";
//        return false;
//    }
//    while (db.next()) {
//        QMap<QString, QVariant> v;
//        for (int i = 0; i < db.columnCount(); i++) {
//            v[db.columnName(i)] = db.value(i);
//        }
//        storeData.append(v);
//    }
//    QList<QMap<QString, QVariant> > queries;
//    for (int i = 0; i < goodsID.count(); i++) {
//        double qty = qtyList.at(i);
//        totalList[i] = 0;
//        for (int j = 0; j < storeData.count(); j++) {
//            if (storeData.at(j)["f_goods"].toInt() == goodsID.at(i).toInt()) {
//                if (storeData.at(j)["f_qty"].toDouble() > 0) {
//                    if (storeData.at(j)["f_qty"].toDouble() >= qty) {
//                        storeData[j]["f_qty"] = storeData.at(j)["f_qty"].toDouble() - qty;
//                        QMap<QString, QVariant> newrec;
//                        newrec[":f_document"] = docId;
//                        newrec[":f_store"] = storeOut;
//                        newrec[":f_type"] = -1;
//                        newrec[":f_goods"] = goodsID.at(i).toInt();
//                        newrec[":f_qty"] = qty;
//                        newrec[":f_price"] = storeData.at(j)["f_price"];
//                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * qty;
//                        newrec[":f_base"] = storeData.at(j)["f_base"].toString();
//                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
//                        newrec[":f_reason"] = reason;
//                        newrec[":f_draft"] = recID.at(i);
//                        newrec[":f_comment"] = comments.at(i);
//                        queries << newrec;
//                        amount += storeData.at(j)["f_price"].toDouble() * qty;
//                        totalList[i] = totalList[i] + (storeData.at(j)["f_price"].toDouble() * qty);
//                        priceList[i] = totalList[i] / qtyList.at(i);
//                        qty = 0;
//                    } else {
//                        QMap<QString, QVariant> newrec;
//                        newrec[":f_document"] = docId;
//                        newrec[":f_store"] = storeOut;
//                        newrec[":f_type"] = -1;
//                        newrec[":f_goods"] = goodsID.at(i).toInt();
//                        newrec[":f_qty"] = storeData.at(j)["f_qty"].toDouble();
//                        newrec[":f_price"] = storeData.at(j)["f_price"].toDouble();
//                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * storeData.at(j)["f_qty"].toDouble();
//                        newrec[":f_base"] = storeData.at(j)["f_base"];
//                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
//                        newrec[":f_reason"] = reason;
//                        newrec[":f_draft"] = recID.at(i);
//                        newrec[":f_comment"] = comments.at(i);
//                        queries << newrec;
//                        totalList[i] = totalList[i] + (storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble());
//                        priceList[i] = totalList[i] / qtyList.at(i);
//                        amount += storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble();
//                        qty -= storeData.at(j)["f_qty"].toDouble();
//                        storeData[j]["f_qty"] = 0.0;
//                    }
//                }
//            }
//            if (qty < 0.00001) {
//                break;
//            }
//        }
//        if (qty > 0.00001) {
//            if (err.isEmpty()) {
//                err += QObject::tr("Not enough materials in the store") + "<br>";
//            }
//            err += QString("%1 - %2").arg(goodsName.at(i)).arg(qty) + "<br>";
//        }
//    }
//    if (err.isEmpty()) {
//        QStringList outId;
//        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.constBegin(); it != queries.constEnd(); it++) {
//            QString newId = QUuid::createUuid().toString().replace("{", "").replace("}", "");
//            outId << newId;
//            db.setBindValues(*it);
//            db[":f_id"] = newId;
//            db.insert("a_store");
//        }

//        db[":f_document"] = docId;
//        db.exec("select f_type, sum(f_total) as f_total from a_store where f_document=:f_document group by 1");
//        if (db.next()) {
//            db[":f_amount"] = db.doubleValue("f_total");
//            db.update("a_header", "f_id", docId);
//        }

//        if (storeIn > 0) {
//            switch (docType) {
//            case 3:
//                foreach (QString recid, outId) {
//                    db[":f_id"] = recid;
//                    db.exec("select * from a_store where f_id=:f_id");
//                    db.next();
//                    db.setBindValues(db.getBindValues());
//                    QString mid = QUuid::createUuid().toString().replace("{", "").replace("}", "");
//                    db[":f_base"] = mid;
//                    db[":f_basedoc"] = db[":f_id"];
//                    db[":f_id"] = mid;
//                    db[":f_document"] = docId;
//                    db[":f_type"] = 1;
//                    db[":f_store"] = storeIn;
//                    if (!db.insert("a_store")) {
//                        err = db.lastDbError();
//                        return false;
//                    }
//                }
//                break;
//            case 6:
//                db[":f_header"] = docId;
//                db.exec("select sum(f_amount) from a_complectation_additions where f_header=:f_header");
//                db.next();
//                amount += db.doubleValue(0);
//                QString id = QUuid::createUuid().toString().replace("{", "").replace("}", "");
//                db[":f_id"] = id;
//                db[":f_document"] = docId;
//                db[":f_store"] = storeIn;
//                db[":f_type"] = 1;
//                db[":f_goods"] = complectCode;
//                db[":f_qty"] = complectQty;
//                db[":f_price"] = amount / complectQty;
//                db[":f_total"] = amount;
//                db[":f_base"] = outId.at(0);
//                db[":f_basedoc"] = docId;
//                db[":f_reason"] = reason;
//                db[":f_draft"] = complectId;
//                if (!db.insert("a_store")) {
//                    err = db.lastDbError();
//                    return false;
//                }
//                break;
//            }
//        }
//        for (int i = 0; i < recID.count(); i++) {
//            db[":f_price"] = priceList.at(i);
//            db[":f_total"] = totalList.at(i);
//            db.update("a_store_draft", "f_id", recID.at(i));
//            if (!baseID.at(i).isEmpty()) {
//                db[":f_price"] = priceList.at(i);
//                db[":f_total"] = totalList.at(i);
//                db.update("a_store_draft", "f_id", baseID.at(i));
//            }
//        }
//        if (docType == 6) {
//            db[":f_type"] = 1;
//            db[":f_document"] = docId;
//            db[":f_price"] = amount / complectQty;
//            db[":f_total"] = amount;
//            db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_type=:f_type and f_document=:f_document");
//        }
//    }

//    return err.isEmpty();
//}

void checkQty(QJsonObject &ord, const QString &orderid, Database &db)
{
    CommandLine cl;
    QString path;
    cl.value("path", path);
    QString configFile = path + "/config.ini";
    QSettings s(configFile, QSettings::IniFormat);

    Database dbsys;
    if (!dbsys.open(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("ElinaShop::checkqty. %1").arg(db.lastDbError()));
        return;
    }
    QMap<QString ,double> qty;
    QStringList goods;
    for (int i = 0; i < ord["items"].toArray().count(); i++) {
        QJsonObject o = ord["items"].toArray().at(i).toObject();
        qty[o["sku"].toString()] = qty[o["sku"].toString()] + o["qty"].toDouble();
        goods.append(o["sku"].toString());
    }
    QMap<int, QMap<QString, double> > out;
    StoreManager::queryQty(goods, out);
    QString storeorder = s.value("shop/storeorder").toString();
    LogWriter::write(LogWriterLevel::verbose, "", QString("Using store list: %1").arg(storeorder));
    if (out.contains(s.value("shop/store").toInt())) {
        QMap<QString, double> store = out[s.value("shop/store").toInt()];
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
        usersStoreMap[dbsys.integer("fid")] = dbsys.integer("fuser");
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
                dbsys[":fsender"] = s.value("shop/store").toInt();
                dbsys[":freceiver"] = usersStoreMap[so.toInt()];
                dbsys[":fmessage"] = QString("%1 - %2 (%4) : %3").arg(iq.key()).arg(iq.value()).arg(QObject::tr("Moved to online store"), orderid);
                dbsys.insert("users_chat");
                qty[iq.key()] = 0;
            } else {
                writeMovement(orderid, StoreManager::codeOfSku(iq.key()), store[iq.key()], so.toInt(), db);

                dbsys[":fdateserver"] = QDateTime::currentDateTime();
                dbsys[":fstate"] = 1;
                dbsys[":fsender"] = s.value("shop/store").toInt();
                dbsys[":freceiver"] = usersStoreMap[so.toInt()];
                dbsys[":fmessage"] = QString("%1 - %2 (%4): %3").arg(iq.key()).arg(store[iq.key()]).arg(QObject::tr("Moved to online store"), orderid);
                dbsys.insert("users_chat");
                iq.value() -= store[iq.key()];
            }
        }
    }
    dbsys.close();
}

bool storerequest(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    CommandLine cl;
    QString path;
    cl.value("path", path);
    QString configFile = path + "/handlers/shop.ini";
    StoreManager::init(configFile);
    ShopManager::init(configFile);
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QStringList fGoods;
    QString fQuery;
    QString message;
    QStringList queryTypes;
    queryTypes.append("qty");
    queryTypes.append("qty2");

    if (!dataMap.contains("key")) {
        message += "Product key missing.";
    } else {
        QString key = getData(indata, dataMap["key"]);
        if (key != "fae292deae2e11eb85290242ac130003") {
            message += "Invalid product key.";
        }
    }
    fQuery.clear();
    if (!dataMap.contains("query")) {
        message += "Query type missing";
    } else {
        for (const QString &qt: queryTypes) {
            if (qt == getData(indata, dataMap["query"])) {
                fQuery = getData(indata, dataMap["query"]);
                break;
            }
        }
        if (fQuery.isEmpty()) {
            message += "Invalid query type";
            fQuery = "";
        }
    }

    if (!dataMap.contains("goods")) {
        message += "Goods list missing.";
    } else {
        fGoods = QString(getData(indata, dataMap["goods"])).split(",", Qt::SkipEmptyParts);
        if (fGoods.isEmpty()) {
            message += "Emtpy goods list";
        }
    }

    JsonHandler jh;
    if (!message.isEmpty()) {
        jh["message"] = message;
        return rh.setDataValidationError(jh.toString());
    }
    //PROCESS
    if (fQuery == "qty") {
        QJsonArray jaGoods;
        QMap<QString, double> gl;
        StoreManager::queryQty(0, fGoods, gl);

        if (fGoods.count() > gl.count()) {
            jh["status"] = "partial";
        }
        for (QMap<QString,double>::const_iterator it = gl.constBegin(); it != gl.constEnd(); it++) {
            QJsonObject jo;
            jo["sku"] = it.key();
            jo["qty"] = it.value();
            jaGoods.append(jo);
        }
        jh["goods"] = jaGoods;
        jh["status"] = gl.count() == 0 ? "empty" : jh["status"].toString().isEmpty() ? "ok" : jh["status"].toString();
    } else if (fQuery == "qty2") {
        QJsonArray jaStore;
        QMap<int, QMap<QString, double> > gl;
        StoreManager::queryQty(fGoods, gl);
        for (QMap<int, QMap<QString,double> >::const_iterator it = gl.constBegin(); it != gl.constEnd(); it++) {
            QJsonArray jaGoods;
            for (QMap<QString, double>::const_iterator it2 = it.value().constBegin(); it2 != it.value().constEnd(); it2++) {
                QJsonObject jo;
                jo["sku"] = it2.key();
                jo["qty"] = it2.value();
                jaGoods.append(jo);
            }
            QJsonObject js;
            js["store"] = it.key();
            js["goods"] = jaGoods;
            jaStore.append(js);
        }
        jh["data"] = jaStore;
        jh["status"] = gl.count() == 0 ? "empty" : jh["status"].toString().isEmpty() ? "ok" : jh["status"].toString();
    }

    return rh.setResponse(HTTP_OK, jh.toString());
}

bool printtax(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATE
    QString fAuthMethod;
    QString fKey;
    QString fUser;
    QString fPassword;
    QString fOrder;
    QString err;
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    if (!dataMap.contains("auth")) {
        err += "Authentication method missing";
    } else {
        QStringList authMethods;
        authMethods.append("up");
        authMethods.append("key");
        for (const QString &am: authMethods) {
            if (getData(indata, dataMap["auth"]) == am) {
                fAuthMethod = am;
                break;
            }
        }
        if (fAuthMethod.isEmpty()) {
            err += "Valid authentication method missing. ";
        } else {
            if (fAuthMethod == "up") {
                if (!dataMap.contains("user")) {
                    err += "Username reqired. ";
                } else {
                    fUser = getData(indata, dataMap["user"]);
                }
                if (!dataMap.contains("pass")) {
                    err += "Password required";
                } else {
                    fPassword = getData(indata, dataMap["pass"]);
                }
            } else if (fAuthMethod == "key") {
                if (!dataMap.contains("key")) {
                    err += "Key required. ";
                } else {
                    fKey = getData(indata, dataMap["key"]);
                }
            }
        }
    }
    if (!dataMap.contains("order")) {
        err += "Order number required. ";
    } else {
        fOrder = getData(indata, dataMap["order"]);
    }
    if (err.isEmpty() == false) {
        return rh.setDataValidationError(err);
    }

    CommandLine cl;
    QString path;
    cl.value("path", path);
    QString configFile = path + "/handlers/shop.ini";
    QSettings s(configFile, QSettings::IniFormat);

    Database db;
    JsonHandler jh;
    if (!db.open(configFile)) {
        jh["message"] = db.lastDbError();
        return rh.setInternalServerError(jh.toString());
    }
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    PrintTaxN pt(s.value("shop/taxip").toString(),
                 s.value("shop/taxport").toInt(),
                 s.value("shop/taxpass").toString(),
                 s.value("shop/taxextpos").toString(),
                 s.value("shop/taxcashier").toString(),
                 s.value("shop/taxpin").toString(),
                 nullptr);
    double amountTotal = 0;
    db[":f_header"] = fOrder;
    db.exec("select gr.f_taxdept, gr.f_adgcode, og.f_qty, og.f_price, g.f_scancode, g.f_name "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_groups gr on gr.f_id=g.f_group "
            "where og.f_header=:f_header");
    while(db.next()) {
        amountTotal += db.doubleValue("f_price") * db.doubleValue("f_qty");
        pt.addGoods(s.value("shop/taxdep").toString(), //dep
                    db.string("f_adgcode"), //adg
                    db.string("f_scancode"), //goods id
                    db.string("f_name"), //name
                    db.doubleValue("f_price"), //price
                    db.doubleValue("f_qty"), //qty
                    0); //discount
    }
    QString jsonIn, jsonOut;
    int result = 0;
    result = pt.makeJsonAndPrint(0, 0, jsonIn, jsonOut, err);
    if (result == pt_err_ok) {
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = fOrder;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = fOrder;
        db[":f_dept"] = s.value("shop/taxdep").toString();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = "(F)";
        db[":f_time"] = time;
        db.insert("o_tax");
    } else {
        return rh.setInternalServerError("<br>" + err + "<br>" + jsonIn);
    }
    jh["rseq"] = rseq;
    return rh.setResponse(HTTP_OK, jh.toString());
}

bool shoprequest(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    CommandLine cl;
    QString path;
    cl.value("path", path);
    QString configFile = path + "/handlers/shop.ini";

    StoreManager::init(configFile);
    ShopManager::init(configFile);
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QJsonObject fOrderJson;
    switch (contentType) {
    case ContentType::ApplilcationJson:
        if (!validateApplicationJson(rh, fOrderJson, indata, dataMap)) {
            return false;
        }
        break;
    default:
        if (!validateMultipartFormData(rh, fOrderJson, indata, dataMap)) {
            return false;
        }
        break;
    }
    //PROCESS
    JsonHandler jh;
    Database db;
    if (!db.open(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("ElinaShop::shoprequest").arg(db.lastDbError()));
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }
    QSettings s(configFile, QSettings::IniFormat);


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
    fOrderJson["items"] = ja;
    qDebug() << fOrderJson;
    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        amountTotal += o["price"].toDouble() * o["qty"].toDouble();
    }
    if (fOrderJson["delivery"].toDouble() > 0) {
        amountTotal += fOrderJson["delivery"].toDouble();
    }

    double amountCash = fOrderJson["payment_type"].toInt() == 1 ? amountTotal : 0;
    double amountCard = fOrderJson["payment_type"].toInt() == 2 ? amountTotal : 0;
    double amountOther = fOrderJson["payment_type"].toInt() > 4 ? amountTotal : 0;
    double amountIDram = fOrderJson["payment_type"].toInt() == 6 ? amountTotal : 0;
    double amountTellCell = fOrderJson["payment_type"].toInt() == 7 ? amountTotal : 0;
    db.startTransaction();
    /* O_draft_sale */
    QString uuid = db.uuid();
    db[":f_id"] = uuid;
    db[":f_state"] = 1;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_staff"] = 1;
    db[":f_amount"] = amountTotal;
    db[":f_payment"] = fOrderJson["payment_type"].toInt();
    db[":f_comment"] = QObject::tr("Delivery address") + ": "
            + fOrderJson["address"].toObject()["city"].toString()
            + ", " + fOrderJson["address"].toObject()["address"].toString()
            + ", " + QObject::tr("phone") + ": "
            + fOrderJson["address"].toObject()["phone"].toString();
    if (!db.insert("o_draft_sale")) {
        LogWriter::write(LogWriterLevel::errors, "", "ElinaShop::requestshop." + db.lastDbError());
        rh.setInternalServerError(db.lastDbError());
        db.rollback();
        return false;
    }


    /* A_STORE_DRAFT, O_GOODS */
    for (int i = 0; i < fOrderJson["items"].toArray().count(); i++) {
        QJsonObject o = fOrderJson["items"].toArray().at(i).toObject();
        QString goodsid = db.uuid();
        db[":f_id"] = goodsid;
        db[":f_header"] = uuid;
        db[":f_state"] = 1;
        db[":f_dateappend"] = QDate::currentDate();
        db[":f_timeappend"] = QTime::currentTime();
        db[":f_goods"] = StoreManager::codeOfSku(o["sku"].toString());
        db[":f_qty"] = o["qty"].toDouble();
        db[":f_price"] = o["price"].toDouble();
        db[":f_userappend"] = 1;
        db.insert("o_draft_sale_body");
    }

    if (fOrderJson["delivery"].toDouble() > 0) {
        QString goodsid = db.uuid();
        db[":f_id"] = goodsid;
        db[":f_header"] = uuid;
        db[":f_state"] = 1;
        db[":f_dateappend"] = QDate::currentDate();
        db[":f_timeappend"] = QTime::currentTime();
        db[":f_goods"] = s.value("shop/delivery").toInt();
        db[":f_qty"] = 1;
        db[":f_price"] = fOrderJson["delivery"].toDouble();
        db[":f_userappend"] = 1;
        db.insert("o_draft_sale_body");
    }
    db.commit();


    jh["status"] = "OK";
    jh["uuid"] = uuid;
    jh["order_id"] = uuid;
    jh["before_store_error"] = "";

    return rh.setResponse(HTTP_OK, jh.toString());
}
