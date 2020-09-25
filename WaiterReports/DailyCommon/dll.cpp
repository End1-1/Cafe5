#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Daily, common");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select count(oh.f_id) as f_qty, sum(oh.f_amounttotal) as f_amounttotal, "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother, oh.f_datecash "
            "from o_header oh "
            "where oh.f_state=:f_state "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by oh.f_datecash "
            "order by oh.f_datecash ");
    C5Printing p;
    p.setSceneParams(700, 2700, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.ctext(QObject::tr("Daily, common"));
    p.br();
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
        p.ltext(QObject::tr("Date"), 5);
        p.rtext(db.getDate(6).toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(QObject::tr("Number of orders"), 5);
        p.rtext(db.getString(0));
        p.br();
        qty += db.getInt(0);
        if (db.getDouble(2) > 0.001) {
            p.ltext(QObject::tr("Cash"), 5);
            p.rtext(float_str(db.getDouble(2), 2));
            p.br();
            cash += db.getDouble(3);
        }
        if (db.getDouble(3) > 0.001) {
            p.ltext(QObject::tr("Card"), 5);
            p.rtext(float_str(db.getDouble(3), 2));
            p.br();
            card += db.getDouble(4);
        }
        if (db.getDouble(4) > 0.001) {
            p.ltext(QObject::tr("Bank"), 5);
            p.rtext(float_str(db.getDouble(4), 2));
            p.br();
            bank += db.getDouble(5);
        }
        if (db.getDouble(5) > 0.001) {
            p.ltext(QObject::tr("Other"), 5);
            p.rtext(float_str(db.getDouble(5), 2));
            p.br();
            other += db.getDouble(6);
            C5Database dbo(db);
            dbo[":f_state"] = ORDER_STATE_CLOSE;
            dbo[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
            dbo[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
            dbo.exec("select p.f_name, count(oh.f_id), sum(oh.f_amountother) as f_amountother "
                    "from o_header oh "
                    "left join o_pay_other p on p.f_id=oh.f_otherid "
                    "where oh.f_state=:f_state and oh.f_otherid>0 "
                    "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
                    "group by 1 "
                    "order by 1 ");
            if (dbo.nextRow()) {
                p.br();
                p.br();
                p.ltext(QObject::tr("Other:"), 25);
                p.br();
                do {
                    p.ltext(dbo.getString(0), 5);
                    p.rtext(QString("%1 (%2)").arg(float_str(dbo.getDouble(2), 2)).arg(dbo.getInt(1)));
                    p.br();
                } while (dbo.nextRow());
            }
        }
        p.setFontBold(true);
        p.ltext(QObject::tr("Total amount"), 5);
        p.rtext(float_str(db.getDouble(1), 2));
        total += db.getDouble(2);
        p.setFontBold(false);
        p.br();
        p.line();
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
    trans.load(":/DailyCommon.qm");
}
