#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

QString caption()
{
    return QObject::tr("Daily by waiters");
}

QJsonArray json(C5Database &db, const QJsonObject &params)
{
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select concat(u.f_last, ' ', u.f_first) as f_staffname, count(oh.f_id), sum(oh.f_amounttotal), "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother "
            "from o_Header oh "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_state=:f_state "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 "
            "order by 1 ");
    C5Printing p;
    p.setSceneParams(700, 2700, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    while (db.nextRow()) {
        p.ltext(db.getString(0), 5);
        p.br();
    }

    return p.jsonData();
}
