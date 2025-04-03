#include "c5printrecipta4.h"
#include "c5database.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5printpreview.h"
#include "QRCodeGenerator.h"

C5PrintReciptA4::C5PrintReciptA4(const QStringList &dbParams, const QString &orderid, QObject *parent) :
    QObject(parent),
    fOrderUUID(orderid),
    fDBParams(dbParams)
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
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    db[":f_id"] = fOrderUUID;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    bool oops = false;
    bool isDraft;
    if (db.nextRow() == false) {
        oops = true;
    }
    if (oops) {
        db[":f_id"] = fOrderUUID;
        db.exec("select * from o_header where f_id=:f_id");
        if (db.nextRow() == false) {
            err = "Order not exists";
            return false;
        }
        isDraft = db.getInt("f_state") != 2;
    } else {
        isDraft = db.getInt("f_state") == 1;
    }
    db[":f_id"] = fOrderUUID;
    if (isDraft) {
        db.exec("select '--' as f_ordernumber, ost.f_name as f_saletypename, "
                "o.f_amount as f_amounttotal, 0 as f_amountcash, 0 as f_amountcard, 0 as f_amountother, o.f_date, "
                "p.f_taxcode, p.f_taxname, p.f_address, o.f_datefor, "
                "concat_ws(' ', u.f_last, u.f_first) as f_staff, "
                "CONCAT(DATE(o.f_date), ' ', TIME(o.f_time)) AS lu, o.f_id, "
                "o.f_partner "
                "from  o_draft_sale o "
                "left join o_sale_type ost on ost.f_id=o.f_saletype "
                "left join s_user u on u.f_id=o.f_staff "
                "left join c_partners p on p.f_id=o.f_partner "
                "where o.f_id=:f_id ");
    } else {
        db.exec("select concat(o.f_prefix, o.f_hallid) as f_ordernumber, ost.f_name as f_saletypename, "
                "o.f_amounttotal, o.f_amountcash, o.f_amountcard, o.f_amountother, o.f_datecash, "
                "p.f_taxcode, p.f_taxname, p.f_address, ds.f_datefor, "
                "concat_ws(' ', u.f_last, u.f_first) as f_staff, "
                "o.lu, o.f_id, o.f_partner  "
                "from o_header o "
                "left join o_draft_sale ds on ds.f_id=o.f_id "
                "left join o_sale_type ost on ost.f_id=o.f_saletype "
                "left join s_user u on u.f_id=o.f_staff "
                "left join c_partners p on p.f_id=o.f_partner "
                "where o.f_id=:f_id ");
    }
    QMap<QString, QVariant> header;
    if (db.nextRow()) {
        db.rowToMap(header);
    } else {
        err = "Order not exists";
        return false;
    }
    QMap<QString, QVariant> debt1, debt2;
    if (header["f_partner"].toInt() > 0) {
        db[":f_costumer"] = header["f_partner"];
        db[":f_order"] = fOrderUUID;
        db.exec("select 0 as a, sum(f_amount) as dd from b_clients_debts "
                "where f_costumer=:f_costumer and f_order<>:f_order "
                "union "
                "select 1 as a, sum(f_amount) as dd from b_clients_debts "
                "where f_costumer=:f_costumer and f_order=:f_order "
                "order by 1");
        db.nextRow();
        db.rowToMap(debt1);
        db.nextRow();
        db.rowToMap(debt2);
    }
    QList<QMap<QString, QVariant> > body;
    db[":f_header"] = fOrderUUID;
    if (isDraft) {
        db.exec("select g.f_scancode, g.f_name as f_goodsname, "
                "ob.f_qty, ob.f_price, ob.f_qty*ob.f_price as f_total, "
                "s.f_name as f_storename, gu.f_name as f_unitname, ob.f_discount as f_discountfactor "
                "from o_draft_sale_body ob "
                "left join c_goods g on g.f_id=ob.f_goods "
                "left join c_storages s on s.f_id=ob.f_store "
                "left join c_units gu on gu.f_id=g.f_unit "
                "where ob.f_header=:f_header "
                "order by ob.f_row ");
    } else {
        db.exec("select g.f_scancode, g.f_name as f_goodsname, ob.f_qty, ob.f_price, ob.f_total, "
                "s.f_name as f_storename, gu.f_name as f_unitname, ob.f_discountfactor * 100 as f_discountfactor "
                "from o_goods ob "
                "left join c_goods g on g.f_id=ob.f_goods "
                "left join c_storages s on s.f_id=ob.f_store "
                "left join c_units gu on gu.f_id=g.f_unit "
                "where ob.f_header=:f_header "
                "order by ob.f_row ");
    }
    while (db.nextRow()) {
        QMap<QString, QVariant> b;
        db.rowToMap(b);
        body.append(b);
    }
    int levelIndex = 1;
    int versionIndex = 0;
    bool bExtent = true;
    int maskIndex = -1;
    QString encodeString = QString("doc;%1;%2;%3").arg(header["f_id"].toString(),
                           isDraft ? tr("Draft") : header["f_ordernumber"].toString(),
                           header["lu"].toString());
    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex,
                              encodeString.toUtf8().data() );
    if (!successfulEncoding) {
        //            fLog.append("Cannot encode qr image");
    }
    int qrImageSize = qrEncode.m_nSymbleSize;
    int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    encodeImage.fill(1);
    for ( int i = 0; i < qrImageSize; i++ ) {
        for ( int j = 0; j < qrImageSize; j++ ) {
            if ( qrEncode.m_byModuleData[i][j] ) {
                encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
            }
        }
    }
    QPixmap pix = QPixmap::fromImage(encodeImage);
    pix = pix.scaled(300, 300);
    p.image(pix, Qt::AlignLeft);
    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText = QString("%1 %2").arg(header["f_saletypename"].toString(), tr("sale"));
    p.ctext(QString("%1 N%2").arg(docTypeText, header["f_ordernumber"].toString()));
    p.br();
    p.fTop = p.fLineHeight + 10;
    p.setFontSize(20);
    p.setFontBold(true);
    points.clear();
    points << 300 << 200 << 500 << 950;
    vals << tr("Date");
    vals << tr("Delivery date");
    vals << tr("Buyer");
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    vals << QDate::fromString(header["f_datecash"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString("dd/MM/yyyy");
    vals << QDate::fromString(header["f_datefor"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString("dd/MM/yyyy");
    vals << QString("%1, %2 %3, %4 %5").arg(header["f_taxname"].toString(), tr("Taxpayer code"),
                                            header["f_taxcode"].toString(),
                                            tr("Address"), header["f_address"].toString());
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    //SALER
    points.clear();
    points << 300 << 200 << 1450;
    vals.clear();
    vals << tr("Saler");
    vals << __c5config.fMainJson["firmfullinfo"].toString();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 300;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 300;
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    QString goodsColName = tr("Goods");
    points.clear();
    points << 50 << 100 << 250 << 800 << 100 << 100 << 150 << 100 << 150 << 150;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Discount")
         << tr("Discounted\r\nprice")
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
        double price = m["f_price"].toDouble();
        vals << float_str(price, 2);
        vals << float_str(m["f_discountfactor"].toDouble(), 2) + "%";
        price -= price *(m["f_discountfactor"].toDouble() / 100);
        vals << float_str(price, 1);
        vals << float_str(m["f_qty"].toDouble() *price, 2);
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }
    p.setFontBold(true);
    points.clear();
    points << 1200
           << 450
           << 300;
    vals.clear();
    vals << tr("Total amount");
    vals << float_str(header["f_amounttotal"].toDouble(), 2);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    if (abs(debt1["dd"].toDouble()) > 0 || abs(debt2["dd"].toDouble()) > 0) {
        p.ltext(tr("Debt before"), 50);
        p.ltext(float_str(debt1["dd"].toDouble(), 1), 400);
        p.br();
        p.ltext(tr("Debt changed"), 50);
        p.ltext(float_str(debt2["dd"].toDouble(), 1), 400);
        p.br();
        p.ltext(tr("Total debt"), 50);
        p.ltext(float_str(debt2["dd"].toDouble() + debt1["dd"].toDouble(), 1), 400);
        p.br(p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    p.ltext(tr("Passed"), 50);
    p.ltext(tr("Accepted"), 1000);
    p.br(p.fLineHeight + 20);
    p.ltext(header["f_staff"].toString(), 50);
    p.line(50, p.fTop, 700, p.fTop);
    p.line(1000, p.fTop, 1650, p.fTop);
    p.setFontBold(false);
    C5PrintPreview pp( &p, fDBParams);
    pp.exec();
    return true;
}
