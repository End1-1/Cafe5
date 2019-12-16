#include "dll.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QJsonArray>

void caption(QString &str)
{
    str = QObject::tr("Daily total report");
}

void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr)
{
    QString shift;
    if (params["f_shift"].toString().toInt() > 0) {
        shift = " and oh.f_shift in (" + params["f_shift"].toString() + ") ";
    }
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select sh.f_name as f_shiftname, count(oh.f_id) as f_qty, sum(oh.f_amounttotal) as f_total, "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother "
            "from o_header oh "
            "left join s_salary_shift sh on sh.f_id=oh.f_shift "
            "where oh.f_state=:f_state " + shift +
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 "
            "order by sh.f_number ");
    C5Printing p;
    p.setSceneParams(700, 2700, QPrinter::Portrait);
    QFont font("Arial LatArm Unicode", 20);
    p.setFont(font);
    p.setFontBold(true);
    p.ctext(QObject::tr("Daily total report"));
    p.setFontBold(false);
    p.br();
    p.ctext(QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR) + " - " + QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR));
    p.br();
    p.br();
    p.line();
    p.setFontSize(24);
    p.setFontBold(true);
    p.ctext(QObject::tr("Orders"));
    p.br();
    while (db.nextRow()) {
        p.br();
        p.setFontBold(true);
        p.ltext(db.getString("f_shiftname"), 0);
        p.br();
        p.setFontBold(false);
        p.ltext(QObject::tr("Number of orders"), 5);
        p.rtext(db.getString("f_qty"));
        p.br();
        if (db.getDouble("f_amountcash") > 0.001) {
            p.ltext(QObject::tr("Cash"), 5);
            p.rtext(float_str(db.getDouble("f_amountcash"), 2));
            p.br();
        }
        if (db.getDouble("f_amountcard") > 0.001) {
            p.ltext(QObject::tr("Card"), 5);
            p.rtext(float_str(db.getDouble("f_amountcard"), 2));
            p.br();
        }
        if (db.getDouble("f_amountbank") > 0.001) {
            p.ltext(QObject::tr("Bank"), 5);
            p.rtext(float_str(db.getDouble("f_amountbank"), 2));
            p.br();
        }
        if (db.getDouble("f_amountother") > 0.001) {
            p.ltext(QObject::tr("Other"), 5);
            p.rtext(float_str(db.getDouble("f_amountother"), 2));
            p.br();
        }
        p.setFontBold(true);
        p.ltext(QObject::tr("Total amount"), 5);
        p.rtext(float_str(db.getDouble("f_total"), 2));
        p.setFontBold(false);
        p.br();
        p.line();
    }
    // TOTAL
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select count(oh.f_id) as f_qty, sum(oh.f_amounttotal) as f_total, "
            "sum(oh.f_amountcash) as f_amountcash, sum(oh.f_amountcard) as f_amountcard, "
            "sum(oh.f_amountbank) as f_amountbank, sum(oh.f_amountother) as f_amountother "
            "from o_header oh "
            "left join s_salary_shift sh on sh.f_id=oh.f_shift "
            "where oh.f_state=:f_state " + shift +
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 ");
    if (db.nextRow()) {
        p.br();
        p.setFontBold(true);
        p.ltext(QObject::tr("Total"), 0);
        p.br();
        p.setFontBold(false);
        p.ltext(QObject::tr("Number of orders"), 5);
        p.rtext(db.getString("f_qty"));
        p.br();
        if (db.getDouble("f_amountcash") > 0.001) {
            p.ltext(QObject::tr("Cash"), 5);
            p.rtext(float_str(db.getDouble("f_amountcash"), 2));
            p.br();
        }
        if (db.getDouble("f_amountcard") > 0.001) {
            p.ltext(QObject::tr("Card"), 5);
            p.rtext(float_str(db.getDouble("f_amountcard"), 2));
            p.br();
        }
        if (db.getDouble("f_amountbank") > 0.001) {
            p.ltext(QObject::tr("Bank"), 5);
            p.rtext(float_str(db.getDouble("f_amountbank"), 2));
            p.br();
        }
        if (db.getDouble("f_amountother") > 0.001) {
            p.ltext(QObject::tr("Other"), 5);
            p.rtext(float_str(db.getDouble("f_amountother"), 2));
            p.br();
        }
        p.setFontBold(true);
        p.ltext(QObject::tr("Total amount"), 5);
        p.rtext(float_str(db.getDouble("f_total"), 2));
        p.setFontBold(false);
        p.br();
        p.line();
    }
    p.br();
    p.line();
    p.br();

    //OTHER PAYMENT DETAILED
    bool f = true;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select sh.f_name as f_shiftname, oh.f_timeclose, po.f_name as f_payothername, oh.f_amountother, "
            "bc.f_govnumber, trim(concat(bcc.f_lastname, ' ', bcc.f_firstname)) as f_costumer "
            "from o_header oh "
            "inner join o_pay_other po on po.f_id=oh.f_otherid "
            "inner join s_salary_shift sh on sh.f_id=oh.f_shift "
            "inner join b_car_orders bco on bco.f_order=oh.f_id "
            "inner join b_car bc on bc.f_id=bco.f_car "
            "inner join b_clients bcc on bcc.f_id=bc.f_costumer "
            "where oh.f_state=:f_state and oh.f_otherid>0 " + shift +
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1 "
            "order by sh.f_number ");
    while (db.nextRow()) {
        if (f) {
            f = false;
            p.setFontBold(true);
            p.br();
            p.ctext(QObject::tr("Other payments"));
            p.br();
            p.setFontBold(false);
        }
        p.ltext(db.getString("f_shiftname") + ", " + db.getTime("f_timeclose").toString("HH:mm") + ", " + db.getString("f_payothername"), 0);
        p.br();
        p.ltext(db.getString("f_costumer"), 0);
        p.br();
        p.ltext(db.getString("f_govnumber"), 0);
        p.br();
        p.ltext(float_str(db.getDouble("f_amountother"), 2), 0);
        p.br();
        p.line();
        p.br();
    }
    p.br();
    p.line();

    //SALED ITEMS
    p.setFontBold(true);
    p.br();
    p.ctext(QObject::tr("Sales"));
    p.br();
    p.setFontBold(false);
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_dishstate"] = DISH_STATE_OK;
    db[":f_datecash1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_datecash2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select sh.f_name as f_shiftname, d.f_name as f_dishname, sum(ob.f_qty1) as f_qty, ob.f_price, sum(ob.f_total) as f_total "
            "from o_body ob "
            "left join o_header oh on oh.f_id=ob.f_header "
            "left join s_salary_shift sh on sh.f_id=oh.f_shift "
            "left join d_dish d on d.f_id=ob.f_dish "
            "where oh.f_state=:f_state " + shift +
            "and ob.f_state=:f_dishstate "
            "and oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by sh.f_name, ob.f_price, d.f_name "
            "order by sh.f_number, d.f_part ");
    QString shiftName;
    while (db.nextRow()) {
        if (shiftName != db.getString("f_shiftname")) {
            p.br();
            shiftName = db.getString("f_shiftname");
            p.setFontBold(true);
            p.ltext(db.getString("f_shiftname"), 0);
            p.setFontBold(false);
            p.br();
        }
        p.ltext(db.getString("f_dishname"), 0);
        p.br();
        p.rtext(QString("%1 X %2 = %3")
                .arg(float_str(db.getDouble("f_qty"), 2))
                .arg(float_str(db.getDouble("f_price"), 2))
                .arg(float_str(db.getDouble("f_total"), 2)));
        p.br();
        p.line();
    }
    p.br();
    p.line();
    p.br();

    //CASH
    p.setFontBold(true);
    p.ctext(QObject::tr("Cashdesk"));
    p.br();
    p.setFontBold(false);
    db[":f_date1"] = QDate::fromString(params["date1"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db[":f_date2"] = QDate::fromString(params["date2"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select c.f_header, cn.f_name as f_cashname, h.f_date, c.f_remarks, c.f_sign, c.f_amount "
            "from e_cash c "
            "inner join e_cash_names cn on cn.f_id=c.f_cash "
            "inner join a_header h on h.f_id=c.f_header and h.f_type=5 "
            "where h.f_date between :f_date1 and :f_date2 "
            "order by cn.f_name, f_sign desc, f_date");
    QString inout, cashname;
    double cashbalance = 0;
    while (db.nextRow()) {
        if (cashname != db.getString("f_cashname")) {
            p.setFontBold(true);
            if (!cashname.isEmpty()) {
                p.ltext(QString("%1 %2").arg(QObject::tr("Balance")).arg(cashname), 0);
                p.br();
                p.rtext(float_str(cashbalance, 2));
                p.br();
                p.line();
                p.br();
            }
            inout.clear();
            cashbalance = 0;
            cashname = db.getString("f_cashname");
            p.br();
            p.ltext(cashname, 0);
            p.br();
            p.setFontBold(false);
        }
        if (inout != db.getString("f_sign")) {
            inout = db.getString("f_sign");
            p.br();
            p.setFontBold(true);
            if (inout == "1") {
                p.ltext(QObject::tr("Input"), 0);
            } else {
                p.ltext(QObject::tr("Output"), 0);
            }
            p.setFontBold(false);
            p.br();
        }
        p.ltext(db.getString("f_remarks"), 0);
        p.br();
        p.rtext(float_str(db.getDouble("f_amount"), 2));
        p.br();
        p.line();
        cashbalance += db.getInt("f_sign") * db.getDouble("f_amount");
    }
    if (!cashname.isEmpty()) {
        p.setFontBold(true);
        p.ltext(QString("%1 %2").arg(QObject::tr("Balance")).arg(cashname), 0);
        p.br();
        p.rtext(float_str(cashbalance, 2));
        p.br();
        p.setFontBold(false);
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
    trans.load(":/DailyForWash.qm");
}
