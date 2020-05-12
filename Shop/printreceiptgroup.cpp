#include "printreceiptgroup.h"
#include "c5printing.h"
#include "c5config.h"
#include "c5utils.h"
#include <QApplication>

PrintReceiptGroup::PrintReceiptGroup(QObject *parent) :
    QObject(parent)
{

}

void PrintReceiptGroup::print(const QString &id, C5Database &db)
{
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    QString saletype;
    if (db.getDouble("f_amounttotal") < 0) {
        saletype = tr("Return");
    }
    QString hallid = db.getString("f_hallid");
    QString pref = db.getString("f_prefix");
    int partner = db.getInt("f_partner");
    double amountTotal = db.getDouble("f_amounttotal");
    double amountCash = db.getDouble("f_amountcash");
    double amountCard = db.getDouble("f_amountcard");
    QString partnerName, partnerTaxcode;
    if (partner > 0) {
        db[":f_id"] = partner;
        db.exec("select * from c_partners where f_id=:f_id");
        if (db.nextRow()) {
            partnerTaxcode = db.getString("f_taxcode");
            partnerName = db.getString("f_taxname");
        }
    }

    db[":f_header"] = id;
    db.exec("select "
            "if (gc.f_base is null, t2.f_name, t1.f_name) as f_goods, "
            "sum(if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty)) as f_qty, "
            "if(gc.f_base is null, ad.f_price, g.f_saleprice2) as f_price, "
            "sum(if(gc.f_base is null, ad.f_price*ad.f_qty, g.f_saleprice2*(gc.f_qty*ad.f_qty))) as f_total "
            "from o_goods ad "
            "left join c_goods_complectation gc on gc.f_base=ad.f_goods "
            "left join c_goods g on g.f_id=gc.f_goods "
            "left join c_goods g2 on g2.f_id=ad.f_goods "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "left join c_groups t1 on t1.f_id=g.f_group "
            "left join c_groups t2 on t2.f_id=g2.f_group "
            "where oh.f_id=:f_header "
            "group by 1, 3 ");
    QList<QList<QVariant> > data;
    while (db.nextRow()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }
        data.append(v);
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
        p.rtext(QString("%1 X %2 = %3")
                .arg(float_str(data.at(i).at(1).toDouble(), 2))
                .arg(data.at(i).at(2).toDouble(), 2)
                .arg(float_str(data.at(i).at(3).toDouble(), 2)));
        p.br();
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
        p.ltext(tr("Payment, cash"), 0);
        p.rtext(float_str(amountCash, 2));
        p.br();
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
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);

}
