#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Daily, removed before precheck");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    qDebug() << params;
    QList<QMap<QString, QVariant> > rremoved;

    //REMOVED ITEMS
    db[":f_state1"] = DISH_STATE_MISTAKE;
    db[":f_state2"] = DISH_STATE_VOID;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select concat(oh.f_prefix, oh.f_hallid) as f_order, t.f_name as f_tablename, d.f_name as f_dishname, ob.f_qty1, ob.f_price, "
            "concat(u.f_last, ' ', u.f_first) as f_staff, bs.f_name as f_statename, ob.f_removereason, ob.f_removetime "
            "from o_body ob "
            "left join o_header oh on oh.f_id=ob.f_header "
            "left join h_tables t on t.f_id=oh.f_table "
            "left join s_user u on u.f_id=ob.f_removeuser "
            "left join d_dish d on d.f_id=ob.f_dish "
            "left join o_body_state bs on bs.f_id=ob.f_state "
            "where oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "and ob.f_state in (:f_state1, :f_state2) and ob.f_precheck=0 "
            "order by 1 ");
    while (db.nextRow()) {
        QMap<QString, QVariant> v;
        db.rowToMap(v);
        rremoved.append(v);
    }

    C5Printing p;
    qDebug() << "Paper width" << params["paper_width"];
    p.setSceneParams(params["paper_width"].toInt(), 2700, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.ctext(QObject::tr("Daily, removed before precheck"));
    p.br();
    p.ctext(QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)
            + " - "
            + QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR));
    p.br();
    p.br();
    p.line();
    p.br();

    if (rremoved.count() > 0) {
        p.br();
        p.br();
        p.ctext(QObject::tr("Removed"));
        p.br();
        p.line();
        p.br();
        for (QList<QMap<QString, QVariant> >::const_iterator it = rremoved.begin(); it != rremoved.end(); it++) {
            const QMap<QString, QVariant> &m = *it;
            p.ctext(QString("%1 - %2 - %3")
                    .arg(m["f_tablename"].toString())
                    .arg(m["f_order"].toString())
                    .arg(m["f_removetime"].toDateTime().toString(FORMAT_DATETIME_TO_STR)));
            p.br();
            p.ltext(m["f_staff"].toString(), 0);
            p.br();
            p.ltext(m["f_statename"].toString() + "/" + m["f_removereason"].toString(), 0);
            p.br();
            p.ltext(m["f_dishname"].toString(), 0);
            p.br();
            p.ltext(QString("%1 X %2 = %3")
                    .arg(m["f_qty1"].toDouble())
                    .arg(float_str(m["f_price"].toDouble(), 2))
                    .arg(float_str(m["f_qty1"].toDouble() * m["f_price"].toDouble(), 2)), 0);
            p.br();
            p.line();
            p.br();

        }
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
    trans.load(":/DailyRemovedTillPrecheck.qm");
}
