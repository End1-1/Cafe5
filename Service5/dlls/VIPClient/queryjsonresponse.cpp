#include "queryjsonresponse.h"
#include "logwriter.h"
#include "printtaxn.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5tr.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QJsonParseError>
#include <QEventLoop>
#include <QNetworkProxy>

#define query_init 1
#define query_create_order 2
#define query_call_function -1
#define query_sql 3
#define query_payment 8

QueryJsonResponse::QueryJsonResponse(Database &db, const QJsonObject &ji, QJsonObject &jo) :
    fDb(db),
    fJsonIn(ji),
    fJsonOut(jo)
{

}

void QueryJsonResponse::getResponse()
{
    fJsonOut["status"] = 1;
    fJsonOut["data"] = "";
#ifdef ELINA
    if (!fJsonIn["config"].toString().isEmpty()) {
        QString sql = QString("select f_config from s_config where f_name='%1'").arg(fJsonIn["config"].toString());
        if (!networkRedirect(sql)) {
            return;
        }
        if (!fJsonOut["data"].toObject()["data"].toArray().isEmpty()) {
            fConfig = QJsonDocument::fromJson(fJsonOut["data"].toObject()["data"].toArray()[0].toString().toUtf8()).object();
        }
    }
#else
    fDb[":f_name"] = fJsonIn["config"].toString();
    fDb.exec("select f_config from s_config where f_name=:f_name");
    if (fDb.next()) {
        fConfig = QJsonDocument::fromJson(fDb.string("f_config").toUtf8()).object();
    }
#endif

    switch (fJsonIn["query"].toInt()) {
    case query_init:
        init();
        break;
    case query_create_order:
        createOrder();
        break;
    case query_call_function:
        callFunction(fJsonIn["function"].toString());
        break;
    case query_sql:
        dbQuery();
        break;
    case query_payment:
        payment();
        break;
    default:
        fJsonOut["status"] = 0;
        fJsonOut["data"] = "Unknown query";
        break;
    }

}

QJsonArray QueryJsonResponse::dbToArray()
{
    QJsonArray ja;
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
    return ja;
}

bool QueryJsonResponse::dbFail(const QString &msg)
{
    fJsonOut["status"] = 0;
    fJsonOut["data"] = msg;
    return false;
}

bool QueryJsonResponse::init()
{
    if (!fDb.exec(QString("select sf_vip_init('%1')").arg(QString(QJsonDocument(fJsonIn["params"].toObject()).toJson())))) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Initialization failed");
    }

    fJsonOut["data"] = QJsonDocument::fromJson(fDb.string(0).toUtf8()).object();
    return true;
}

bool QueryJsonResponse::callFunction(const QString &name)
{
    QJsonObject jo = fJsonIn["params"].toObject();
    if (!fDb.exec(QString("select %1('%2')")
                  .arg(name)
                  .arg(QString(QJsonDocument(jo).toJson())))) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Call function failed");
    }
    fJsonOut = QJsonDocument::fromJson(fDb.string(0).toUtf8()).object();
    return true;
}

bool QueryJsonResponse::createOrder()
{
    QJsonObject jo = fJsonIn["params"].toObject();
    if (!jo.contains("f_staff")) {
        jo["f_staff"] = 1;
    }
    if (!fDb.exec(QString("select sf_create_order('%1')")
                  .arg(QString(QJsonDocument(jo).toJson())))) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Create order failed");
    }
    fJsonOut = QJsonDocument::fromJson(fDb.string(0).toUtf8()).object();
    return true;
}

bool QueryJsonResponse::dbQuery()
{
#ifdef ELINA
    return networkRedirect(fJsonIn["sql"].toString());
#endif
    QElapsedTimer t;
    t.start();
    if (!fDb.exec(fJsonIn["sql"].toString())) {
        return dbFail(fDb.lastDbError());
    }
    qDebug() << "db executed in " << t.elapsed();
    QJsonObject jrep;
    jrep["columns"] = fDb.columnsData();
    QJsonArray jdata;
    QJsonObject jtype;
    bool first = true;
    while (fDb.next()) {
        QJsonArray ja;
        for (int i = 0; i < fDb.columnCount(); i++) {
            switch (fDb.value(i).type()) {
            case QVariant::Int:
                ja.append(fDb.value(i).toInt());
                if (first) {
                    jtype[QString::number(i)] = QVariant::Int;
                }
                break;
            case QVariant::Double:
                ja.append(fDb.value(i).toDouble());
                if (first) {
                    jtype[QString::number(i)] = QVariant::Double;
                }
                break;
            case QVariant::Date:
                ja.append(fDb.value(i).toDate().toString("dd-MM-yyyy"));
                if (first) {
                    jtype[QString::number(i)] = QVariant::Date;
                }
                break;
            case QVariant::Time:
                ja.append(fDb.value(i).toTime().toString("HH:mm:ss"));
                if (first) {
                    jtype[QString::number(i)] = QVariant::Time;
                }
                break;
            case QVariant::DateTime:
                ja.append(fDb.value(i).toDateTime().toString("dd-MM-yyyy HH:mm:ss"));
                if (first) {
                    jtype[QString::number(i)] = QVariant::DateTime;
                }
                break;
            default:
                ja.append(fDb.value(i).toString());
                if (first) {
                    jtype[QString::number(i)] = QVariant::String;
                }
                break;
            }
        }
        first = false;
        jdata.append(ja);
    }
    qDebug() << "Data fetched in" << t.elapsed();
    jrep["data"] = jdata;
    jrep["types"] = jtype;
    fJsonOut["data"] = jrep;
    return true;
}

bool QueryJsonResponse::networkRedirect(const QString &sql)
{
    QElapsedTimer t;
    t.start();
    QString server = QString("");
    QEventLoop loop;
    QNetworkAccessManager m;
    QNetworkRequest rq(server);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m.setProxy(QNetworkProxy::NoProxy);
    // QSslConfiguration sslConf = rq.sslConfiguration();
    // sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    // sslConf.setProtocol(QSsl::AnyProtocol);
    // rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["sk"] = "5cfafe13-a886-11ee-ac3e-1078d2d2b808";
    jo["call"] = "sql";
    jo["sql"] = sql;
    auto *r = m.post(rq, QJsonDocument(jo).toJson());
    r->connect(r, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (r->error() != QNetworkReply::NoError) {
        LogWriter::write(LogWriterLevel::verbose, "", QString("Elapsed: %1, query: %2, error: %3").arg(QString::number(t.elapsed()), sql, r->errorString()));
        return dbFail(r->errorString());
    }
    fJsonOut = QJsonDocument::fromJson(r->readAll()).object();
    QJsonArray ja = fJsonOut["data"].toObject()["types"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        switch (ja[i].toInt()) {
        case 3:
            ja[i] = QVariant::Int;
            break;
        case 246:
            ja[i] = QVariant::Double;
            break;
        default:
            ja[i] = QVariant::String;
            break;
        }
    }
    fJsonOut["data"].toObject()["types"] = ja;
    r->deleteLater();
    LogWriter::write(LogWriterLevel::verbose, "", QString("Elapsed: %1, query: %2").arg(QString::number(t.elapsed()), sql));
    qDebug() << t.elapsed() << sql;
    return true;
}

bool QueryJsonResponse::payment()
{
    QElapsedTimer et;
    et.start();
    QJsonObject jo = fJsonIn["params"].toObject();

    PrintTaxN pt(fConfig["fiscal_ip"].toString(),
                 fConfig["fiscal_port"].toInt(),
                 fConfig["fiscal_password"].toString(),
                 fConfig["fiscal_extpos"].toString(),
                 fConfig["fiscal_opcode"].toString(),
                 fConfig["fiscal_oppin"].toString());

    fDb[":f_id"] = jo["f_id"].toString();
    if (!fDb.exec("select * from o_header where f_id=:f_id")) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Order not exists");
    }
    double amountcard = fDb.doubleValue("f_amountcard") + fDb.doubleValue("f_amountidram");

    fDb[":f_header"] = jo["f_id"].toString();
    if (!fDb.exec("select d.f_name, coalesce(d.f_adgt, p.f_adgcode) as f_adgcode, "
                  "b.f_qty1, b.f_price, b.f_discount, p.f_taxdept, d.f_id "
                  "from o_body b "
                  "left join d_dish d on d.f_id=b.f_dish "
                  "left join d_part2 p on p.f_id=d.f_part "
                  "left join o_header o on o.f_id=b.f_header "
                  "where b.f_state=1 and o.f_state=2 and o.f_id=:f_header ")) {
        return dbFail(fDb.lastDbError());
    }
    if (fDb.rowCount() == 0) {
        return dbFail("Empty order");
    }

    while (fDb.next()) {
        pt.addGoods(fDb.integer("f_taxdept"),
                    fDb.string("f_adgcode"),
                    fDb.string("f_id"),
                    fDb.string("f_name"),
                    fDb.doubleValue("f_price"),
                    fDb.doubleValue("f_qty1"),
                    fDb.doubleValue("f_discount"));
    }
    QString inJson, outJson, err;
    int fiscal_result = pt.makeJsonAndPrint(amountcard, 0, inJson, outJson, err);
    QJsonObject jtax;
    QJsonParseError jsonErr;
    jtax["f_order"] = jo["f_id"].toString();
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(inJson.toUtf8(), &jsonErr).object();
    jtax["f_out"] = QJsonDocument::fromJson(outJson.toUtf8()).object();;
    jtax["f_err"] = err;
    jtax["f_result"] = fiscal_result;
    jtax["f_state"] = fiscal_result == pt_err_ok ? 1 : 0;
    if (jsonErr.error != QJsonParseError::NoError) {
        fJsonOut["data"] = jsonErr.errorString();
    }
    if (!fDb.exec(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact))))) {
        return dbFail(fDb.lastDbError());
    }
    if (fiscal_result != pt_err_ok) {
        return dbFail(err);
    }

    if (!fDb.exec(QString("select %1('%2')")
                      .arg("sf_order_payment",
                           QString(QJsonDocument(jo).toJson())))) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Call function failed");
    }
    return printBill(jo["f_id"].toString());
}

bool QueryJsonResponse::printBill(const QString &id)
{
    int bs = fConfig["recipe_font_size"].toInt();
    C5Tr __translator;
    __translator.setLanguage(fJsonIn["language"].toString());
    fDb.exec("select * from s_translator");
    while (fDb.next()) {
        __translator.add(fDb.string("f_en"), fDb.string("f_am"), fDb.string("f_ru"));
    }
    QFont font(fConfig["recipe_font_family"].toString());
    font.setPointSize(bs);
    C5Printing p;
    p.setSceneParams(fConfig["recipe_paper_width"].toInt(), 2600, QPrinter::Portrait);
    p.setFont(font);

    QMap<QString, QVariant> header;
    QList<QMap<QString, QVariant>> body;
    QMap<QString, QVariant> options;
    QJsonObject taxinfo;
    QMap<QString, QVariant> clinfo;
    QMap<QString, QVariant> roominfo;
    QMap<QString, QVariant> preorder;
    QMap<QString, QVariant> carinfo;
    QMap<QString, QVariant> giftcardinfo;

    fDb[":f_id"] = id;
    if (!fDb.exec("select h.f_prefix, h.f_hallid, concat_ws(' ', u.f_last, u.f_first)as f_currentstaffname, t.f_name as f_tablename, hl.f_name as f_hallname, "
                  "h.f_amounttotal, h.f_amountcash, h.f_amountcard, h.f_amountidram, h.f_amounttelcell, h.f_amountbank, h.f_amountpayx, "
                  "h.f_amountservice, h.f_servicefactor, h.f_discountfactor, h.f_amountdiscount, "
                  "h.f_state, h.f_comment "
                  "from o_header h "
                  "left join h_tables t on t.f_id=h.f_table "
                  "left join h_halls hl on hl.f_id=t.f_hall "
                  "left join s_user u on u.f_id=h.f_currentstaff "
                  "where h.f_id=:f_id ")) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Order not exists");
    }
    fDb.rowToMap(header);

    fDb[":f_header"] = id;
    if (!fDb.exec("select d.f_name as f_name_am, dt.f_en as f_name_en, dt.f_ru as f_name_ru,"
                  "b.f_qty1, b.f_qty2, b.f_price, b.f_canservice, b.f_candiscount, b.f_state, "
                  "d.f_hourlypayment, d.f_extra, b.f_comment, b.f_total "
                  "from o_body b "
                  "left join d_dish d on d.f_id=b.f_dish "
                  "left join d_translator dt on dt.f_id=d.f_id "
                  "where b.f_header=:f_header ")) {
        return dbFail(fDb.lastDbError());
    }
    while (fDb.next()) {
        QMap<QString, QVariant> r;
        fDb.rowToMap(r);
        body.append(r);
    }

    fDb[":f_id"] = id;
    if (!fDb.exec("select * from o_header_options where f_id=:f_id")) {
        return dbFail(fDb.lastDbError());
    }
    if (!fDb.next()) {
        return dbFail("Order not exists");
    }
    fDb.rowToMap(options);

    fDb[":f_id"] = id;
    if (!fDb.exec("select * from o_preorder where f_id=:f_id")) {
        return dbFail("Order not exists");
    }
    if (fDb.next()) {
        fDb.rowToMap(preorder);
    }

    QMap<QString, QVariant> guestInfo;
    if (header["f_partner"].toInt() > 0) {
        fDb[":f_id"] = header["f_partner"];
        fDb.exec("select * from c_partners where f_id=:f_id");
        if (fDb.next()) {
            fDb.rowToMap(guestInfo);
        }
    }

    fDb[":f_order"] = id;
    if (!fDb.exec("select * from o_tax_log where f_state=1 and f_order=:f_order")) {
        return dbFail("Order not exists");
    }
    if (fDb.next()) {
        taxinfo = QJsonDocument::fromJson(fDb.string("f_out").toUtf8()).object();
    }

    fDb[":f_id"] = id;
    if (!fDb.exec("select * from o_pay_cl where f_id=:f_id")) {
        return dbFail("Order not exists");
    }
    if (fDb.next()) {
        fDb.rowToMap(clinfo);
    }


    fDb[":f_id"] = id;
    if (!fDb.exec("select * from o_pay_room where f_id=:f_id")) {
        return dbFail("Order not exists");
    }
    if (fDb.next()) {
        fDb.rowToMap(roominfo);
    }

    fDb[":f_id"] = options["f_car"].toInt();
    if (!fDb.exec("select * from b_car where f_id=:f_id")) {
        return dbFail("Order not exists");
    }
    if (fDb.next()) {
        fDb.rowToMap(carinfo);
    }

    fDb[":f_trsale"] = id;
    fDb.exec("select h.f_value, i.f_amount from b_gift_card_history i "
            "left join b_history h on h.f_id=i.f_trsale "
            "where h.f_id=:f_trsale");
    if (fDb.next()) {
        fDb.rowToMap(giftcardinfo);
    }
    fDb[":f_trsale"] = id;
    fDb.exec("select c.f_code, concat(p.f_contact) as f_costumer, sum(h.f_amount) as f_totalamount  "
            "from b_gift_card_history h "
            "left join b_cards_discount c on c.f_id=h.f_card "
            "left join c_partners p on p.f_id=c.f_client "
            "where h.f_card in (select f_card from b_gift_card_history where f_trsale=:f_trsale)");
    fDb.next();
    fDb.rowToMap(giftcardinfo);

    p.image("./logo_receipt.png", Qt::AlignHCenter);

    p.br();
    p.setFontBold(true);
    p.ctext(__translator.tt("Receipt #") + QString("%1%2").arg(header["f_prefix"].toString()).arg(header["f_hallid"].toString()));
    p.br();
    if (header["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
        || header["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ctext(__translator.tt("Preorder"));
        p.br();
        p.br(4);
        p.setFontSize(bs);
        p.setFontBold(false);
    }
    p.setFontBold(false);
    if (taxinfo["rseq"].toInt() > 0) {
        p.ltext(taxinfo["taxpayer"].toString(), 0);
        p.br();
        p.ltext(taxinfo["address"].toString(), 0);
        p.br();
//        p.ltext(__translator.tt("Department"), 0);
//        p.rtext(taxinfo["f_dept"].toString());
//        p.br();
        p.ltext(__translator.tt("Tax number"), 0);
        p.rtext(taxinfo["tin"].toString());
        p.br();
        p.ltext(__translator.tt("Device number"), 0);
        p.rtext(taxinfo["crn"].toString());
        p.br();
        p.ltext(__translator.tt("Serial"), 0);
        p.rtext(taxinfo["sn"].toString());
        p.br();
        p.ltext(__translator.tt("Fiscal"), 0);
        p.rtext(taxinfo["fiscal"].toString());
        p.br();
        p.ltext(__translator.tt("Receipt number"), 0);
        p.rtext(QString::number(taxinfo["rseq"].toInt()));
        p.br();
//        p.ltext(__translator.tt("Date"), 0);
//        p.rtext(taxinfo["f_taxtime"].toString());
//        p.br();
        p.ltext(__translator.tt("(F)"), 0);
        p.br();
    }
    p.br(1);
    p.ltext(__translator.tt("Table"), 0);
    p.setFontSize(bs);
    p.setFontBold(true);
    p.rtext(QString("%1/%2").arg(header["f_hallname"].toString(), header["f_tablename"].toString()));
    p.setFontSize(bs);
    p.setFontBold(false);
    p.br();
    p.ltext(__translator.tt("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(__translator.tt("Staff"), 0);
    p.rtext(header["f_currentstaffname"].toString());
    p.br();
    if (options["f_guests"].toInt() > 0) {
        p.ltext(__translator.tt("Guests"), 0);
        p.rtext(options["f_guests"].toString());
        p.br();
    }
    p.br(2);
    p.line();
    p.br(2);
    p.br(4);
    p.line();
    p.br(1);

    double needtopay = header["f_amounttotal"].toDouble();
    bool noservice = false, nodiscount = false;
    p.setFontBold(true);
    p.setFontSize(bs - 4);
    p.ltext(__translator.tt("Name"), 0);
    p.ltext(__translator.tt("Qty"), 330);
    p.ltext(__translator.tt("Price"), 410);
    p.rtext(__translator.tt("Amount"));
    p.br();
    p.br(2);
    p.line();
    p.br(1);
    for (int i = 0; i < body.count(); i++) {
        QMap<QString, QVariant> &m = body[i];
        if (m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        QString name;
        if (!m["f_adgcode"].toString().isEmpty()) {
            p.ltext(QString("%1: %2").arg(__translator.tt("Class"), m["f_adgcode"].toString()), 0);
            p.br();
        }
        name = m["f_name_" + fJsonIn["language"].toString()].toString();
        if (m["f_canservice"].toInt() == 0) {
            noservice = true;
            name += "*";
        }
        if (m["f_candiscount"].toInt() == 0) {
            nodiscount = true;
            name += "**";
        }
        p.ltext(name, 0, 330);

        if (header["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
            || header["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
            m["f_qty2"] = m["f_qty1"];
            m["f_total"] = m["f_qty1"].toDouble() * m["f_price"].toDouble();
            double srvAmount = 0;
            if (m["f_canservice"].toInt() != 0) {
                srvAmount = (m["f_total"].toDouble() * header["f_servicefactor"].toDouble());
            }
            needtopay += m["f_total"].toDouble() + srvAmount;
            header["f_amountservice"] = header["f_amountservice"].toDouble() + srvAmount;
            header["f_amounttotal"] = needtopay;
        }

        QString totalStr = m["f_total"].toDouble() > 0.001 ? float_str(m["f_total"].toDouble(), 2) : __translator.tt("Present");
        if (str_float(totalStr) < 0.001) {
            p.rtext(totalStr);
        } else {
            if (m["f_hourlypayment"].toInt() > 0) {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.rtext(float_str(m["f_total"].toDouble(), 2));
            } else {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.ltext(float_str(m["f_price"].toDouble(), 2), 410, 120);
                p.rtext(totalStr);
            }
        }
        if (m["f_extra"].toInt() > 0) {
            p.br();
            p.ltext(m["f_comment"].toString(), 0);
        }
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.br();
    p.ltext(__translator.tt("Total"), 0);
    double printTotal = header["f_amounttotal"].toDouble()
                        + header["f_amountdiscount"].toDouble()
                        - header["f_amountservice"].toDouble();
    p.rtext(float_str(printTotal, 2));
    p.br();
    p.br(1);

    if (header["f_amountservice"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Service")).arg(header["f_servicefactor"].toDouble() * 100), 0);
        p.rtext(float_str(header["f_amountservice"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    if (header["f_discountfactor"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Discount"), float_str(header["f_discountfactor"].toDouble() * 100, 2)), 0);
        p.rtext(float_str(header["f_amountdiscount"].toDouble(), 2));
        p.br();
        p.br(1);
    }

    //CARDINFO
    if (giftcardinfo.count() > 0) {
        p.ltext(QString("%1 %2% (%3)").arg(__translator.tt("Accumulate"),
                                           float_str(giftcardinfo["f_value"].toDouble(), 2),
                                           float_str(giftcardinfo["f_amount"].toDouble(), 2)), 0);
        p.rtext(float_str(giftcardinfo["f_totalamount"].toDouble(), 2));
        p.br();
        //        p.ltext(QString("%1 %2").arg(__translator.tt("Card"), giftcardinfo["f_code"].toString()), 0);
        //        p.br();
        p.ltext(giftcardinfo["f_costumer"].toString(), 0);
        p.br();
    }

    double prepaid = preorder["f_prepaidcash"].toDouble()
                     + preorder["f_prepaidcard"].toDouble()
                     + preorder["f_prepaidpayx"].toDouble();
    if (prepaid > 0){
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Prepaid amount"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(prepaid * -1, 2));
        needtopay -= prepaid;
        p.br();
        p.br();
    }

    p.br();
    p.line();
    p.br(2);
    p.line();
    p.br();

    p.setFontSize(bs + 4);
    if (needtopay > 0.01) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Need to pay"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(needtopay, 2));
    } else {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Refund"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(needtopay, 2));
    }
    p.br();
    p.br();
    p.line();
    p.setFontBold(true);
    p.setFontSize(bs - 4);
    if (noservice) {
        p.ltext(QString("* - %1").arg(__translator.tt("No service")), 0);
        p.br();
    }
    if (nodiscount) {
        p.ltext(QString("** - %1").arg(__translator.tt("No discount")), 0);
        p.br();
    }
    p.br();
    p.line();

//    if (fIdram[param_idram_id].length() > 0 && fBill){
//        C5LogSystem::writeEvent("Idram QR");
//        p.br();
//        p.br();
//        p.br();
//        p.br();
//        p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
//        p.br();

//        int levelIndex = 1;
//        int versionIndex = 0;
//        bool bExtent = true;
//        int maskIndex = -1;
//        QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
//                                   .arg(fIdram[param_idram_name])
//                                   .arg(fIdram[param_idram_id]) //IDram ID
//                                   .arg(fHeaderInfo["f_amounttotal"].toDouble())
//                                   .arg(fHeader)
//                                   .arg(fIdram[param_idram_phone])
//                                   .arg(fIdram[param_idram_tips].toInt() == 1 ? "1" : "0")
//                                   .arg(fIdram[param_idram_tips].toInt() == 1 ? fIdram[param_idram_tips_wallet] : "");

//        CQR_Encode qrEncode;
//        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
//        if (!successfulEncoding) {
//            //            fLog.append("Cannot encode qr image");
//        }
//        int qrImageSize = qrEncode.m_nSymbleSize;
//        int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
//        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
//        encodeImage.fill(1);

//        for ( int i = 0; i < qrImageSize; i++ ) {
//            for ( int j = 0; j < qrImageSize; j++ ) {
//                if ( qrEncode.m_byModuleData[i][j] ) {
//                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
//                }
//            }
//        }

//        QPixmap pix = QPixmap::fromImage(encodeImage);
//        pix = pix.scaled(300, 300);
//        p.image(pix, Qt::AlignHCenter);
//        p.br();
//        /* End QRCode */
//    } else {

//    }


    if (header["f_amountcash"].toDouble() > 0.001) {
        double vp = header["f_amountcash"].toDouble();
        p.ltext(__translator.tt("Payment, cash"), 0);
        p.rtext(float_str(vp, 2));
        p.br();
    }
    if (header["f_amountcard"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Payment, card"), 0);
        p.rtext(float_str(header["f_amountcard"].toDouble(), 2));
        p.br();
    }
    if (header["f_amountbank"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Bank transfer"), 0);
        p.rtext(float_str(header["f_amountbank"].toDouble(), 2));
        p.br();
    }
    if (header["f_amountidram"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Idram"), 0);
        p.rtext(float_str(header["f_amountidram"].toDouble(), 2));
        p.br();
    }
    if (header["f_amountpayx"].toDouble() > 0.001) {
        p.ltext(__translator.tt("PayX"), 0);
        p.rtext(float_str(header["f_amountpayx"].toDouble(), 2));
        p.br();
    }
//    if (fHeaderInfo["f_cash"].toDouble() > 0.01) {
//        p.ltext(__translator.tt("Received cash"), 0);
//        p.rtext(float_str(fHeaderInfo["f_cash"].toDouble(), 2));
//        p.br();
//        p.ltext(__translator.tt("Change"), 0);
//        p.rtext(float_str(fHeaderInfo["f_change"].toDouble(), 2));
//        p.br();
//    }
    p.br();


    p.setFontSize(bs + 2);

    if (!roominfo["f_res"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt("Transfer to room"));
        p.br();
        p.ctext(roominfo["f_room"].toString() + ", " + roominfo["f_guest"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt("Signature"));
        p.br(p.fLineHeight * 2);
    }

    if (!clinfo["f_code"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt("City ledger"));
        p.br();
        p.ctext(clinfo["f_code"].toString() + ", " + clinfo["f_name"].toString());
        p.br(p.fLineHeight * 3);
    }

//    if (fHeaderInfo["f_otherid"].toInt() == PAYOTHER_DEBT) {
//        p.br();
//        p.ctext(__translator.tt("Debt") + " " + clinfo["f_name"].toString());
//        p.br(p.fLineHeight * 3);
//        p.line(3);
//        p.ctext(__translator.tt("Signature"));
//        p.br(p.fLineHeight * 2);
//    }

//    if (fHeaderInfo["f_otherid"].toInt() == PAYOTHER_COMPLIMENTARY) {
//        p.br();
//        p.ctext(__translator.tt("Complimentary") + " " + fHeaderInfo["f_comment"].toString());
//        p.br(p.fLineHeight * 3);
//        p.line(3);
//        p.ctext(__translator.tt("Signature"));
//        p.br(p.fLineHeight * 2);
//    }

    if (!guestInfo.isEmpty()) {
        p.ctext(__translator.tt("Customer"));
        p.br();
        p.ctext(guestInfo["f_phone"].toString());
        p.br();
        p.ctext(guestInfo["f_contact"].toString());
        p.br();
        p.ctext(guestInfo["f_address"].toString());
        p.br();
    }

    p.setFontSize(bs);
    p.setFontBold(true);
    //p.ltext(__translator.tt("Thank you for visit!"), 0);
    p.br();
    p.ltext(QString("%1: %2").arg(__translator.tt("Sample")).arg(abs(header["f_precheck"].toInt()) + 1), 0);
    p.br();

    if (!header["f_comment"].toString().isEmpty()) {
        p.br();
        p.br();
        p.br();
        p.br();
        p.br();
        p.line();
        p.br();
        p.ltext(header["f_comment"].toString(), 0);
        p.br();
    }

    p.print(fConfig["recipe_printer"].toString(), QPrinter::Custom);

    return true;
}

