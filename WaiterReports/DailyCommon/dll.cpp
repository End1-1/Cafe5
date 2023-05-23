#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Daily, general");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    qDebug() << params;
    QMap<QString, QVariant> rtotal;
    QList<QMap<QString, QVariant> > rotherdetails;
    QList<QMap<QString, QVariant> > rstaff;

    //GENERAL
    db[":f_state1"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select count(oh.f_id) as f_count, sum(oh.f_amounttotal) as f_amounttotal, "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother, "
            "sum(oh.f_amountidram) as f_amountidram,  "
            "sum(oh.f_amountpayx) as f_amountpayx,  "
            "sum(oh.f_hotel) as f_hotel "
            "from o_header oh "
            "where oh.f_state=:f_state1 "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 ");
    if (db.nextRow()) {
        db.rowToMap(rtotal);
    }

    //OTHER
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select p.f_name, count(oh.f_id), sum(oh.f_amountother) as f_amountother "
            "from o_header oh "
            "left join o_pay_other p on p.f_id=oh.f_otherid "
            "where oh.f_state=:f_state and oh.f_otherid>0 "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 "
            "order by 1 ");
    while (db.nextRow()) {
        QMap<QString, QVariant> v;
        db.rowToMap(v);
        rotherdetails.append(v);
    }

    //BY STAFF
    db[":f_state1"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select concat(u.f_last, ' ', u.f_first) as f_staff, count(oh.f_id) as f_count, "
            "sum(oh.f_amounttotal) as f_amounttotal, "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother, "
            "sum(oh.f_amountidram) as f_amountidram,  "
            "sum(oh.f_amountpayx) as f_amountpayx, "
            "sum(oh.f_hotel) as f_hotel, "
            "sum(oh.f_amountservice) as f_service "
            "from o_header oh "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_state=:f_state1 "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 ");
    while (db.nextRow()) {
        QMap<QString, QVariant> v;
        db.rowToMap(v);
        rstaff.append(v);
    }

    C5Printing p;
    qDebug() << "Paper width" << params["paper_width"];
    //p.setSceneParams(550, 2700, QPrinter::Portrait);
    p.setSceneParams(params["paper_width"].toInt(), 2700, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.ctext(QObject::tr("Daily, general"));
    p.br();
    p.ctext(QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)
            + " - "
            + QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR));
    p.br();
    p.br();
    p.line();
    p.br();

    p.ltext(QObject::tr("Number of orders"), 5);
    p.rtext(rtotal["f_count"].toString());
    p.br();
    p.ltext(QObject::tr("Total amount"), 5);
    p.rtext(float_str(rtotal["f_amounttotal"].toDouble(), 2));
    p.br();
    if (rtotal["f_amountcash"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Cash"), 5);
        p.rtext(float_str(rtotal["f_amountcash"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_amountcard"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Card"), 5);
        p.rtext(float_str(rtotal["f_amountcard"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_amountbank"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Bank"), 5);
        p.rtext(float_str(rtotal["f_amountbank"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_amountidram"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Idram"), 5);
        p.rtext(float_str(rtotal["f_amountidram"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_amountpayx"].toDouble() > 0.001) {
        p.ltext(QObject::tr("PayX"), 5);
        p.rtext(float_str(rtotal["f_amountpayx"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_amountother"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Other"), 5);
        p.rtext(float_str(rtotal["f_amountother"].toDouble(), 2));
        p.br();
    }
    if (rtotal["f_hotel"].toDouble() > 0.001) {
        p.ltext(QObject::tr("Hotel"), 5);
        p.rtext(float_str(rtotal["f_hotel"].toDouble(), 2));
        p.br();
    }

    p.br();
    p.br();
    p.ctext(QObject::tr("By staff"));
    p.br();
    p.line();
    p.br();

    for (QList<QMap<QString, QVariant> >::const_iterator it = rstaff.begin(); it != rstaff.end(); it++) {
        const QMap<QString, QVariant> &m = *it;
        p.ctext(m["f_staff"].toString());
        p.br();
        if (m["f_amountcash"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Cash"), 5);
            p.rtext(float_str(m["f_amountcash"].toDouble(), 2));
            p.br();
        }
        if (m["f_amountcard"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Card"), 5);
            p.rtext(float_str(m["f_amountcard"].toDouble(), 2));
            p.br();
        }
        if (m["f_amountbank"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Bank"), 5);
            p.rtext(float_str(m["f_amountbank"].toDouble(), 2));
            p.br();
        }
        if (rtotal["f_amountidram"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Idram"), 5);
            p.rtext(float_str(rtotal["f_amountidram"].toDouble(), 2));
            p.br();
        }
        if (rtotal["f_amountpayx"].toDouble() > 0.001) {
            p.ltext(QObject::tr("PayX"), 5);
            p.rtext(float_str(rtotal["f_amountpayx"].toDouble(), 2));
            p.br();
        }
        if (m["f_amountother"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Other"), 5);
            p.rtext(float_str(m["f_amountother"].toDouble(), 2));
            p.br();
        }
        if (m["f_hotel"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Hotel"), 5);
            p.rtext(float_str(m["f_hotel"].toDouble(), 2));
            p.br();
        }
        if (m["f_service"].toDouble() > 0.001) {
            p.ltext(QObject::tr("Service"), 5);
            p.rtext(float_str(m["f_service"].toDouble(), 2));
            p.br();
        }
        p.line();
        p.br();
    }

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
