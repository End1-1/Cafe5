#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Orders");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select concat(oh.f_prefix, '', oh.f_hallid) as f_id,t.f_name as tablename, oh.f_dateclose, oh.f_timeclose, oh.f_amounttotal, "
            "oh.f_amountcash, oh.f_amountcard, oh.f_amountbank, oh.f_amountother "
            "from o_header oh "
            "left join h_tables t on t.f_id=oh.f_table "
            "where oh.f_state=:f_state "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "order by oh.f_datecash, oh.f_dateclose, oh.f_timeclose ");
    C5Printing p;
    p.setSceneParams(600, 2800, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.ctext(QObject::tr("Orders"));
    p.br();
    p.ctext(QObject::tr("Date range"));
    p.br();
    //p.ctext(params["date1"].toString() + " - " + params["date2"].toString());
    p.ctext(QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR) + " - " + QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR));
    p.br();
    p.br();
    p.line();
    int qty = 0;
    double total = 0.0,
            cash = 0.0,
            card = 0.0,
            bank = 0.0,
            other = 0.0;
    while (db.nextRow()) {
        p.br();
        p.ltext(QString("%1/%2/%3 %3")
                .arg(db.getString("f_id"))
                .arg(db.getString("tablename"))
                .arg(db.getDate("f_dateclose").toString(FORMAT_DATE_TO_STR))
                .arg(db.getTime("f_timeclose").toString(FORMAT_TIME_TO_STR)), 5);
        p.rtext(float_str(db.getDouble("f_amounttotal"), 2));
        p.br();
        qty ++;
        total += db.getDouble("f_amounttotal");
        if (db.getDouble("f_amountcash") > 0.001) {
            cash += db.getDouble("f_amountcash");
        }
        if (db.getDouble("f_amountcard") > 0.001) {
            card += db.getDouble("f_amountcard");
        }
        if (db.getDouble("f_amountbank") > 0.001) {
            bank += db.getDouble("f_amountbank");
        }
        if (db.getDouble("f_amountother") > 0.001) {
            other += db.getDouble("f_amountother");
        }
    }
    p.br();
    p.line();
    p.br();

    p.setFontSize(24);
    p.ltext(QObject::tr("Total order quantity"), 0);
    p.rtext(QString("%1").arg(qty));
    p.br();
    if (cash > 0.001) {
        p.ltext(QObject::tr("Total cash"), 5);
        p.rtext(float_str(cash, 2));
        p.br();
    }
    if (card > 0.001) {
        p.ltext(QObject::tr("Total card"), 5);
        p.rtext(float_str(card, 2));
        p.br();
    }
    if (bank > 0.001) {
        p.ltext(QObject::tr("Total bank"), 5);
        p.rtext(float_str(bank, 2));
        p.br();
    }
    if (other > 0.001) {
        p.ltext(QObject::tr("Total other"), 5);
        p.rtext(float_str(other, 2));
        p.br();
    }
    p.setFontBold(true);
    p.ltext(QObject::tr("Grand total"), 5);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();
    p.setFontBold(false);
    p.setFontSize(18);
    p.ltext(QString("%1: %2").arg(QObject::tr("Printed")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 5);
    p.br();
    p.br();
    p.line();

    jarr = p.jsonData();
}

void translator(QTranslator &trans)
{
    trans.load(":/DailyOrders.qm");
}
