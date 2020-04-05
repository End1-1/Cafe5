#include "sockethandleruploadsale.h"
#include "c5database.h"
#include "config.h"
#include "c5config.h"
#include "c5storedraftwriter.h"
#include "servicecommands.h"
#include "c5utils.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SocketHandlerUploadSale::SocketHandlerUploadSale(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerUploadSale::processData()
{
    QJsonObject jo = QJsonDocument::fromJson(fData).object();
    QJsonObject jh = jo["h"].toObject();
    QJsonArray jb = jo["b"].toArray();
    qDebug() << jo;

    QDateTime hdatetime = QDateTime::currentDateTime();
    QStringList add = jh["add"].toString().split("|", QString::SkipEmptyParts);
    QString latlon = "0.0|0.0";
    if (add.count() > 1) {
        latlon = QString("%1|%2").arg(add.at(0)).arg(add.at(1));
    }
    if (add.count() > 2) {
        if (add.at(3).toDouble() > 100000) {
            hdatetime = QDateTime::fromMSecsSinceEpoch(add.at(3).toDouble());
        }
    }
    QDate hdate = hdatetime.date();
    QTime htime = hdatetime.time();

    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    db[":f_taxcode"] = jh["taxcode"].toString();
    db.exec("select f_id from c_partners where f_taxcode=:f_taxcode");
    int customer = 0;
    if (db.nextRow()) {
        customer = db.getInt(0);
    }

    QString id = db.uuid();
    db.startTransaction();
    db[":f_id"] = fSocketData->getValue(param_default_hall);
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    QString pref, hallid;
    if (db.nextRow()) {
        hallid = db.getString(0);
        pref = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(fSocketData->getValue(param_default_hall)));
    }

    db[":f_id"] = id;
    db[":f_hallid"] = hallid.toInt();
    db[":f_prefix"] = pref;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = fSocketData->getValue(param_default_hall);
    db[":f_table"] = fSocketData->getValue(param_default_table);
    db[":f_dateopen"] = hdate;
    db[":f_dateclose"] = hdate;
    db[":f_datecash"] = hdate;
    db[":f_timeopen"] = htime;
    db[":f_timeclose"] = htime;
    db[":f_staff"] = fSocketData->fUserId;
    db[":f_comment"] = "";
    db[":f_print"] = 1;
    db[":f_amountTotal"] = jh["atotal"].toDouble();
    db[":f_amountCash"] = jh["acash"].toDouble();
    db[":f_amountCard"] = 0;
    db[":f_amountBank"] = jh["abank"].toDouble();
    db[":f_amountOther"] = jh["adept"].toDouble();
    db[":f_amountService"] = 0;
    db[":f_amountDiscount"] = 0;
    db[":f_serviceMode"] = 0;
    db[":f_serviceFactor"] = 0;
    db[":f_discountFactor"] = 0;
    db[":f_creditCardId"] = 0;
    db[":f_otherId"] = 0;
    db[":f_source"] = 2;
    if (db.insert("o_header", false) == 0) {
        db.rollback();
        db.close();
        fResponseCode = dr_err;
        return;
    }

    db[":f_id"] = id;
    db[":f_coordinates"] = latlon;
    db[":f_customer"] = customer;
    db.insert("o_additional", false);

    for (int i = 0; i < jb.count(); i++) {
        jo = jb.at(i).toObject();
        db[":f_id"] = C5Database::uuid();
        db[":f_header"] = id;
        db[":f_store"] = fSocketData->getValue(param_default_store);
        db[":f_goods"] = jo["goods"].toInt();
        db[":f_qty"] = jo["qty"].toDouble();
        db[":f_price"] = jo["price"].toDouble();
        db[":f_total"] = jo["qty"].toDouble() * jo["зкшсу"].toDouble();
        db[":f_tax"] = 0;
        db[":f_sign"] = 1;
        db[":f_taxdept"] = 0;
        db[":f_row"] = i + 1;
        if (db.insert("o_goods", false) == 0) {
            db.rollback();
            db.close();
            fResponseCode = dr_err;
            return;
        }
    }

    bool fModeRefund = false;
    C5StoreDraftWriter dw(db);
    if (fModeRefund) {
        if (!dw.writeFromShopInput(hdate, id)) {
            db.rollback();
            db.close();
            fResponseCode = dr_err;
            return;
        }
    } else {
        if (!dw.writeFromShopOutput(fSocketData->fUserId, hdate, id)) {
            db.rollback();
            db.close();
            fResponseCode = dr_err;
            return;
        }
    }
    db.commit();
    jo = QJsonObject();
    jo["uuid"] = id;
    jo["id"] = jh["unid"].toString();
    fData = QJsonDocument(jo).toJson(QJsonDocument::Compact);
}
