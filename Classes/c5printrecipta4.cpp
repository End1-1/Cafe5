#include "c5printrecipta4.h"
#include "c5database.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5printpreview.h"

C5PrintReciptA4::C5PrintReciptA4(const QStringList &dbParams, const QString &orderid, QObject *parent) :
    QObject(parent),
    fDBParams(dbParams),
    fOrderUUID(orderid)
{

}

bool C5PrintReciptA4::print(QString &err)
{
    C5Printing p;
    QFont f(__c5config.getValue(param_app_font_family));
    C5Database db(fDBParams);
    p.setFont(f);
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPrinter::Portrait);

    db[":f_id"] = fOrderUUID;
    db.exec("select concat(o.f_prefix, o.f_hallid) as f_ordernumber, ost.f_name as f_saletypename, "
            "o.f_amounttotal, o.f_amountcash, o.f_amountcard, o.f_amountother, o.f_datecash, "
            "p.f_taxcode, p.f_taxname, p.f_address, ds.f_datefor, concat_ws(' ', u.f_last, u.f_first) as f_staff "
            "from o_header o "
            "left join o_draft_sale ds on ds.f_id=o.f_id "
            "left join o_sale_type ost on ost.f_id=o.f_saletype "
            "left join s_user u on u.f_id=o.f_staff "
            "left join c_partners p on p.f_id=o.f_partner "
            "where o.f_id=:f_id ");
    QMap<QString, QVariant> header;
    if (db.nextRow()) {
        db.rowToMap(header);
    } else {
        err = "Order not exists";
        return false;
    }

    QList<QMap<QString, QVariant> > body;
    db[":f_header"] = fOrderUUID;
    db.exec("select g.f_scancode, g.f_name as f_goodsname, ob.f_qty, ob.f_price, ob.f_total, "
            "s.f_name as f_storename, gu.f_name as f_unitname, ob.f_discountfactor * 100 as f_discountfactor "
            "from o_goods ob "
            "left join c_goods g on g.f_id=ob.f_goods "
            "left join c_storages s on s.f_id=ob.f_store "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where ob.f_header=:f_header "
            "order by ob.f_row ");
    while (db.nextRow()) {
        QMap<QString, QVariant> b;
        db.rowToMap(b);
        body.append(b);
    }
    QString store = body.at(0)["f_storename"].toString();

    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText = QString("%1 %2").arg(header["f_saletypename"].toString()).arg(tr("sale"));

    p.ctext(QString("%1 N%2").arg(docTypeText, header["f_ordernumber"].toString()));
    p.br();
    p.br();
    p.setFontSize(20);
    p.setFontBold(false);

    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200 << 500 << 1200;
    vals << tr("Date");
    vals << tr("Delivery date");
    vals << tr("Buyer");

    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    vals << header["f_datecash"].toDate().toString("dd/MM/yyyy");
    vals << header["f_datefor"].toDate().toString("dd/MM/yyyy");;
    vals << QString("%1, %2 %3, %4 %5").arg(header["f_taxname"].toString(), tr("Taxpayer code"), header["f_taxcode"].toString(),
            tr("Address"), header["f_address"].toString());
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    //SALER
    points.clear();
    points << 50 << 200 << 1700;
    vals.clear();
    vals << tr("Saler");
    vals << QString::fromUtf8("\"Հրաչ Աջեմյան\" ԱԶ, ՀՎՀՀ՝ 35136541");
    //vals << QString::fromUtf8("");
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);


    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 50;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 50;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    p.br();
    p.br();



    QString goodsColName = tr("Goods");

    points.clear();
    points << 50 << 100 << 250 << 850 << 100 << 100 << 200 << 100 << 200;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Discount")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < body.count(); i++) {
        const QMap<QString, QVariant> &m = body.at(i);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
            p.br();
        }
        vals.clear();
        vals << QString::number(i + 1);
        vals << m["f_scancode"].toString();
        vals << m["f_goodsname"].toString();
        vals << float_str(m["f_qty"].toDouble(), 2);
        vals << m["f_unitname"].toString();
        vals << float_str(m["f_price"].toDouble(), 2);
        vals << float_str(m["f_discountfactor"].toDouble(), 2) + "%";
        vals << float_str(m["f_total"].toDouble(), 2);
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }

    p.setFontBold(true);
    points.clear();
    points << 1250
           << 500
           << 200;
    vals.clear();
    vals << tr("Total amount");
    vals << float_str(header["f_amounttotal"].toDouble(), 2);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    p.ltext(tr("Passed"), 50);
    p.ltext(tr("Accepted"), 1000);
    p.br(p.fLineHeight + 20);
    p.ltext(header["f_staff"].toString(), 50);


    p.line(50, p.fTop, 700, p.fTop);
    p.line(1000, p.fTop, 1650, p.fTop);
    p.setFontBold(false);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
    return true;
}
