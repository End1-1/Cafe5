#include "printreceiptgroup.h"
#include "c5printing.h"
#include <QApplication>

#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))

PrintReceiptGroup::PrintReceiptGroup(QObject *parent) :
    QObject(parent)
{

}

void PrintReceiptGroup::print(const QString &id, Database &db, int rw)
{
    double cash = 0;
    double change = 0;
    db[":f_id"] = id;
    db.exec("select f_cash, f_change from o_payment where f_id=:f_id");
    if (db.next()) {
        cash = db.doubleValue("f_cash");
        change = db.doubleValue("f_change");
    }

    Database dtax(db);
    dtax[":f_id"] = id;
    dtax.exec("select * from o_tax where f_id=:f_id");

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.next();
    QString saletype;
    if (db.doubleValue("f_amounttotal") < 0) {
        saletype = tr("Return");
    }
    QString hallid = db.string("f_hallid");
    QString pref = db.string("f_prefix");
    int partner = db.integer("f_partner");
    double amountTotal = db.doubleValue("f_amounttotal");
    double amountCash = db.doubleValue("f_amountcash");
    double amountCard = db.doubleValue("f_amountcard");
    QString partnerName, partnerTaxcode;
    if (partner > 0) {
        db[":f_id"] = partner;
        db.exec("select * from c_partners where f_id=:f_id");
        if (db.next()) {
            partnerTaxcode = db.string("f_taxcode");
            partnerName = db.string("f_taxname");
        }
    }

    QString price1, price2;
    switch (rw) {
    case 1:
        price1 = "ad.f_price";
        price2 = "g.f_saleprice2";
        break;
    case 2:
        price1 = "g.f_saleprice";
        price2 = "g.f_saleprice";
        break;
    }

    //Print blocks
    amountTotal = 0;
    db[":f_header"] = id;
    db.exec(QString("select "
            "if (gc.f_base is null, t2.f_name, t1.f_name) as f_goods, "
            "sum(if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty)) as f_qty, "
            "if(gc.f_base is null, %1, %2) as f_price, "
            "sum(if(gc.f_base is null, %1*ad.f_qty, %2*(gc.f_qty*ad.f_qty))) as f_total "
            "from o_goods ad "
            "left join c_goods_complectation gc on gc.f_base=ad.f_goods "
            "left join c_goods g on g.f_id=gc.f_goods "
            "left join c_goods g2 on g2.f_id=ad.f_goods "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "left join c_groups t1 on t1.f_id=g.f_group "
            "left join c_groups t2 on t2.f_id=g2.f_group "
            "where oh.f_id=:f_header and g2.f_unit=10 "
            "group by 1, 3 ").arg(price1).arg(price2));
    QList<QList<QVariant> > data;
    while (db.next()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.value(i));
        }
        data.append(v);
        amountTotal += db.doubleValue("f_total");
    }

    switch (rw) {
    case 1:
        price1 = "ad.f_price";
        break;
    case 2:
        price1 = "g.f_saleprice";
        break;
    }
    db[":f_header"] = id;
    db.exec(QString("select t1.f_name as f_goods, sum(ad.f_qty) as f_qty, %1,  sum(%1*ad.f_qty) as f_total "
            "from o_goods ad "
            "left join c_goods g on g.f_id=ad.f_goods "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "left join c_groups t1 on t1.f_id=g.f_group "
            "where oh.f_id=:f_header and g.f_unit<>10 "
            "group by 1, 3 ").arg(price1));
    while (db.next()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.value(i));
        }
        data.append(v);
        amountTotal += db.doubleValue("f_total");
    }

    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.br(2);
    if (!saletype.isEmpty()) {
        p.setFontSize(22);
        p.ctext(saletype);
        p.br();
        p.setFontSize(20);
    }
    if (dtax.next()) {
        p.ltext(dtax.string("f_firmname"), 0);
        p.br();
        p.ltext(dtax.string("f_address"), 0);
        p.br();
        p.ltext(tr("Department"), 0);
        p.rtext(dtax.string("f_dept"));
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(dtax.string("f_hvhh"));
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(dtax.string("f_devnum"));
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(dtax.string("f_serial"));
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(dtax.string("f_fiscal"));
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(dtax.string("f_receiptnumber"));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(dtax.string("f_time"));
        p.br();
        p.ltext(tr("(F)"), 0);
    }
    if (!partnerName.isEmpty()) {
        p.ltext(tr("Buyer taxcode"), 0);
        p.rtext(partnerTaxcode);
        p.br();
        p.ltext(partnerName, 0);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(QString("#%1%2").arg(pref).arg(hallid));
    p.br();
    p.setFontSize(20);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);
    for (int i = 0; i < data.count(); i++) {
//        if (__c5config.value(param_shop_print_goods_qty_side_down).toInt() == 1) {
//            p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
//            p.br();
//            p.ltext(QString("%1 X %2 = %3")
//                    .arg(float_str(data.at(i).at(1).toDouble(), 3))
//                    .arg(data.at(i).at(2).toDouble(), 2)
//                    .arg(float_str(data.at(i).at(3).toDouble(), 2)), 0);
//            p.br();
//        }
        if (1 == 1) {
            p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
            p.rtext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(1).toDouble(), 3))
                    .arg(data.at(i).at(2).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble(), 2)));
            p.br();
        }
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);

    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(amountTotal, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    if (amountCash > 0.001) {
        if (cash > 0.001) {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(cash, 2));
            p.br();
            if (change > 0.001) {
                p.ltext(tr("Change"), 0);
                p.rtext(float_str(-1 * change, 2));
                p.br();
            }
        } else {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(amountTotal, 2));
            p.br();
        }
    }
    if (amountCard > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(amountCard, 2));
        p.br();
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();

    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    p.br();
    p.print("local", QPrinter::Custom);

}

void PrintReceiptGroup::print2(const QString &id, Database &db, const QString &printerName)
{
    double cash = 0;
    double change = 0;
    double idram = 0;
    double tellcell = 0;
    db[":f_id"] = id;
    db.exec("select f_cash, f_change, f_card, f_idram, f_tellcell from o_payment where f_id=:f_id");
    if (db.next()) {
        cash = db.doubleValue("f_cash");
        change = db.doubleValue("f_change");
        idram = db.doubleValue("f_idram");
        tellcell = db.doubleValue("f_tellcell");
    }

    Database dtax(db);
    dtax[":f_id"] = id;
    dtax.exec("select * from o_tax where f_id=:f_id");

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.next();
    QString saletype;
    if (db.doubleValue("f_amounttotal") < 0) {
        saletype = tr("Return");
    }
    QString hallid = db.string("f_hallid");
    QString pref = db.string("f_prefix");
    int partner = db.integer("f_partner");
    double amountTotal = db.doubleValue("f_amounttotal");
    double amountCash = db.doubleValue("f_amountcash");
    double amountCard = db.doubleValue("f_amountcard");
    QString comment = db.string("f_comment");
    QString partnerName, partnerTaxcode;
    if (partner > 0) {
        db[":f_id"] = partner;
        db.exec("select * from c_partners where f_id=:f_id");
        if (db.next()) {
            partnerTaxcode = db.string("f_taxcode");
            partnerName = db.string("f_taxname");
        }
    }

    QString price1, price2;
    int rw = 1;
    switch (rw) {
    case 1:
        price1 = "ad.f_price";
        price2 = "g.f_saleprice2";
        break;
    case 2:
        price1 = "g.f_saleprice";
        price2 = "g.f_saleprice";
        break;
    }

    //Print blocks
    double tt = 0;
    db[":f_header"] = id;
    db.exec(QString("select "
            "if (gc.f_base is null, g2.f_scancode, g.f_scancode) as f_goods, "
            "sum(if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty)) as f_qty, "
            "if(gc.f_base is null, %1, %2) as f_price, "
            "sum(if(gc.f_base is null, %1*ad.f_qty, %2*(gc.f_qty*ad.f_qty))) as f_total "
            "from o_goods ad "
            "left join c_goods_complectation gc on gc.f_base=ad.f_goods "
            "left join c_goods g on g.f_id=gc.f_goods "
            "left join c_goods g2 on g2.f_id=ad.f_goods "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "left join c_groups t1 on t1.f_id=g.f_group "
            "left join c_groups t2 on t2.f_id=g2.f_group "
            "where oh.f_id=:f_header and g2.f_unit=10 "
            "group by 1, 3 ").arg(price1).arg(price2));
    QList<QList<QVariant> > data;
    while (db.next()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.value(i));
        }
        data.append(v);
        tt += db.doubleValue("f_total");
    }

    switch (rw) {
    case 1:
        price1 = "ad.f_price";
        break;
    case 2:
        price1 = "g.f_saleprice";
        break;
    }
    db[":f_header"] = id;
    QString goodsNameField = "g.f_name as f_goods";
    //if (__c5config.value(param_print_scancode_with_name).toInt() == 1) {
        goodsNameField = "concat(g.f_name, ' ', g.f_scancode) as f_goods";
   // }
    db.exec(QString("select %1, sum(ad.f_qty) as f_qty, %2,  sum(%2*ad.f_qty) as f_total "
            "from o_goods ad "
            "left join c_goods g on g.f_id=ad.f_goods "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "left join c_groups t1 on t1.f_id=g.f_group "
            "where oh.f_id=:f_header and g.f_unit<>10 "
            "group by 1, 3 ").arg(goodsNameField).arg(price1));
    while (db.next()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.value(i));
        }
        data.append(v);
        tt += db.doubleValue("f_total");
    }

    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.br(2);
    if (!saletype.isEmpty()) {
        p.setFontSize(22);
        p.ctext(saletype);
        p.br();
        p.setFontSize(20);
    }

    if (dtax.next()) {
        p.ltext(dtax.string("f_firmname"), 0);
        p.br();
        p.ltext(dtax.string("f_address"), 0);
        p.br();
        p.ltext(tr("Department"), 0);
        p.rtext(dtax.string("f_dept"));
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(dtax.string("f_hvhh"));
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(dtax.string("f_devnum"));
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(dtax.string("f_serial"));
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(dtax.string("f_fiscal"));
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(dtax.string("f_receiptnumber"));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(dtax.string("f_time"));
        p.br();
        p.ltext(tr("(F)"), 0);
    }

    if (!partnerName.isEmpty()) {
        p.ltext(tr("Buyer taxcode"), 0);
        p.rtext(partnerTaxcode);
        p.br();
        p.ltext(partnerName, 0);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(QString("#%1%2").arg(pref).arg(hallid));
    p.br();
    p.setFontSize(20);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);
    for (int i = 0; i < data.count(); i++) {
        p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3")
                .arg(float_str(data.at(i).at(1).toDouble(), 3))
                .arg(data.at(i).at(2).toDouble(), 2)
                .arg(float_str(data.at(i).at(3).toDouble(), 2)), 0);
        p.br();
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);

    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(tt, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    if (amountCash > 0.001) {
        if (cash > 0.001) {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(cash, 2));
            p.br();
            if (change > 0.001) {
                p.ltext(tr("Change"), 0);
                p.rtext(float_str(-1 * change, 2));
                p.br();
            }
        } else {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(amountTotal, 2));
            p.br();
        }
    }
    if (idram > 0.001) {
        p.ltext(tr("Payment, idram"), 0);
        p.rtext(float_str(idram, 2));
        p.br();
    }
    if (tellcell > 0.001){
        p.ltext(tr("Payment, TellCell"), 0);
        p.rtext(float_str(tellcell, 2));
        p.br();
    }

    if (amountCard > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(amountCard, 2));
        p.br();
    }

    if (!comment.isEmpty()) {
        p.br();
        p.ltext(comment, 0);
        p.br();
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();

    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    p.br();
    p.print(printerName, QPrinter::Custom);
}
