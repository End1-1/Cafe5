#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Dishes sales");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    QList<QMap<QString, QVariant> > rdish;
    db[":f_state1"] = ORDER_STATE_CLOSE;
    db[":f_state2"] = DISH_STATE_OK;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select d.f_name as f_dishname, sum(ob.f_qty1) as f_qty, sum(ob.f_total) as f_total "
            "from o_body ob "
            "left join o_header oh on oh.f_id=ob.f_header "
            "left join d_dish d on d.f_id=ob.f_dish "
            "where oh.f_state=:f_state1 and ob.f_state=:f_state2 "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 "
            "order by 2 desc ");
    while (db.nextRow()) {
        QMap<QString, QVariant> m;
        db.rowToMap(m);
        rdish.append(m);
    }
    C5Printing p;
    p.setSceneParams(600, 2800, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.ctext(QObject::tr("Dishes sales"));
    p.br();
    p.ctext(QObject::tr("Date range"));
    p.br();
    p.ctext(QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR) + " - " + QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR));
    p.br();
    p.br();
    p.line();

    for (QList<QMap<QString, QVariant> >::const_iterator it = rdish.begin(); it != rdish.end(); it++) {
        const QMap<QString, QVariant> &m = *it;
        p.ltext(m["f_dishname"].toString(), 0);
        p.rtext(QString("%1 - %2").arg(m["f_qty"].toDouble()).arg(float_str(m["f_total"].toDouble(), 2)));
        p.br();
        p.line();
        p.br();
    }
    p.br();
    p.ltext(QString("%1: %2").arg(QObject::tr("Printed")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 5);

    jarr = p.jsonData();
}

void translator(QTranslator &trans)
{
    trans.load(":/DailyDishes.qm");
}
