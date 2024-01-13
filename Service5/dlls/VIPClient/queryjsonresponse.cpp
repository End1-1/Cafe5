#include "queryjsonresponse.h"
#include "logwriter.h"
#include "printtaxn.h"
#include "c5printing.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
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

    fDb[":f_name"] = fJsonIn["config"].toString();
    fDb.exec("select f_config from s_config where f_name=:f_name");
    if (fDb.next()) {
        fConfig = QJsonDocument::fromJson(fDb.string("f_config").toUtf8()).object();
    }

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
        return dbFail("Create ordre failed");
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
    QString server = QString("https://%1/info.php").arg("aws.elina.am");
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
    jtax["f_order"] = jo["f_id"].toString();
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(inJson.toUtf8()).object();
    jtax["f_out"] = QJsonDocument::fromJson(outJson.toUtf8()).object();;
    jtax["f_err"] = err;
    jtax["f_result"] = fiscal_result;
    jtax["f_state"] = fiscal_result == pt_err_ok ? 1 : 0;
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
    printBill();

    return true;
}

bool QueryJsonResponse::printBill()
{
    return true;
}

