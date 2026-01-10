#include "printreceiptgroup.h"
#include "c5printing.h"
#include "c5config.h"
#include "c5utils.h"
#include "oheader.h"
#include "cpartners.h"
#include "QRCodeGenerator.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>

PrintReceiptGroup::PrintReceiptGroup(QObject *parent) :
    QObject(parent)
{
}

void PrintReceiptGroup::print(const QString &id, C5Database &db, int rw)
{
    double cash = 0;
    double change = 0;
    double totalQty = 0;
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");

    if(db.nextRow()) {
        cash = db.getDouble("f_cash");
        change = db.getDouble("f_change");
    } else {
    }

    int hallid = db.getInt("f_hallid");
    QString pref = db.getString("f_prefix");
    int partner = db.getInt("f_partner");
    double amountTotal = db.getDouble("f_amounttotal");
    double amountCash = db.getDouble("f_amountcash");
    double amountCard = db.getDouble("f_amountcard");
    double amountPrepaid = db.getDouble("f_amountprepaid");
    QString comment = db.getString("f_comment");
    QJsonObject jtax;
    db[":f_order"] = id;
    db.exec("select * from o_tax_log where f_order=:f_order and f_state=1");

    if(db.nextRow()) {
        jtax = QJsonDocument::fromJson(db.getString("f_out").toUtf8()).object();
    }

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    QString saletype;
    QMap<QString, QVariant> returnFrom;

    if(db.getDouble("f_amounttotal") < 0) {
        saletype = tr("Return");
        db[":f_header"] = id;
        db.exec("select f_returnfrom from o_goods where f_header=:f_header");

        if(db.nextRow()) {
            db[":f_id"] = db.getString("f_returnfrom");
            db.exec("select f_header from o_goods where f_id=:f_id");
            db.nextRow();
            db[":f_id"] = db.getString("f_header");
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
            db.rowToMap(returnFrom);
            db[":f_id"] = id;
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
        }
    }

    QString partnerName, partnerTaxcode;

    if(partner > 0) {
        db[":f_id"] = partner;
        db.exec("select * from c_partners where f_id=:f_id");

        if(db.nextRow()) {
            partnerTaxcode = db.getString("f_taxcode");
            partnerName = db.getString("f_taxname");
        }
    }

    QString price1, price2, price1disc, price2disc, pprice1, pprice1disc;

    switch(rw) {
    case 1:
        price1 = "ad.f_price";
        price2 = "gpr.f_price2";
        price1disc = "gpr.f_price2disc";
        pprice1 = "ad.f_price";
        pprice1disc = "ad.f_price";
        break;

    case 2:
        price1 = "gpr.f_price1";
        price2 = "gpr.f_price1";
        price1disc = "gpr.f_price1disc";
        pprice1 = "gpr.f_price1";
        pprice1disc = "gpr.f_price1disc";
        break;
    }

    //Print blocks
    amountTotal = 0;
    db[":f_header"] = id;
    db.exec(QString(R"(select
                    if (gc.f_base is null, t2.f_name, t1.f_name) as f_goods,
                    sum(if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty)) as f_qty,
                    if(gc.f_base is null, if(%1>0,%1, %2), if(%3>0,%3,%4)) as f_price,
                    sum(if(gc.f_base is null, if(%1>0,%1, %2)*ad.f_qty,
                    if(%3>0,%3,%4)*(gc.f_qty*ad.f_qty))) as f_total %5,
                    ad.f_row
                    from o_goods ad
                    left join c_goods_complectation gc on gc.f_base=ad.f_goods
                    left join c_goods g on g.f_id=gc.f_goods
                    left join c_goods g2 on g2.f_id=ad.f_goods
                    left join c_goods_prices gpr on gpr.f_goods=g.f_id
                    inner join o_header oh on oh.f_id=ad.f_header
                    left join c_groups t1 on t1.f_id=g.f_group
                    left join c_groups t2 on t2.f_id=g2.f_group
                    where oh.f_id=:f_header and g2.f_unit=10
                    group by 1, 3 %6

                    union

                    select t1.f_name as f_goods,
                    sum(ad.f_qty) as f_qty,
                    if(%7>0,%7,%8) as f_price,
                    sum(if(%7>0,%7,%8)*ad.f_qty) as f_total %9,
                    ad.f_row
                    from o_goods ad
                    left join c_goods g on g.f_id=ad.f_goods
                    inner join o_header oh on oh.f_id=ad.f_header
                    left join c_groups t1 on t1.f_id=g.f_group
                    left join c_goods_prices gpr on gpr.f_goods=g.f_id
                    where oh.f_id=:f_header and g.f_unit<>10
                    group by 1, 3 %9

                    order by f_row )")
            .arg(price1disc)
            .arg(price1)
            .arg(price1disc)
            .arg(price2)
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? "" : ",ad.f_id")
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? "" : ",5")
            .arg(pprice1disc)
            .arg(pprice1)
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? "" : ",5"));
    QList<QList<QVariant> > data;

    while(db.nextRow()) {
        QList<QVariant> v;

        for(int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }

        data.append(v);
        amountTotal += db.getDouble("f_total");
    }

    QFont font(qApp->font());
    const int bs = 12;
    font.setPointSize(bs);
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(C5Config::localReceiptPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(font);
    p.br(2);

    if(!saletype.isEmpty()) {
        p.ctext(saletype);
        p.br();
    }

    if(jtax["rseq"].toInt() > 0) {
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        //        p.ltext(tr("Date"), 0);
        //        p.rtext(dtax.getString("f_time"));
        //        p.br();
        p.ltext(tr("(F)"), 0);

        if(__c5config.getValue(param_vat).toDouble() > 0.01) {
            p.ltext(QString("%1 %2%").arg(tr("Including VAT"), float_str(__c5config.getValue(param_vat).toDouble() * 100, 2)), 0);
            p.rtext(float_str(amountTotal * __c5config.getValue(param_vat).toDouble(), 2));
            p.br();
        }
    }

    if(!partnerName.isEmpty() || !partnerTaxcode.isEmpty()) {
        p.ltext(tr("Buyer taxcode"), 0);
        p.rtext(partnerTaxcode);
        p.br();
        p.ltext(partnerName, 0);
        p.br();
    }

    p.setFontBold(true);
    p.ctext(QString("#%1%2").arg(pref, QString::number(hallid)));
    p.br();

    if(returnFrom.count() > 0) {
        p.ctext(QString("(%1 %2%3)").arg(tr("Return from"), returnFrom["f_prefix"].toString()).arg(
                    returnFrom["f_hallid"].toInt()));
        p.br();
    }

    p.setFontSize(bs - 2);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);

    for(int i = 0; i < data.count(); i++) {
        if(__c5config.getValue(param_shop_print_goods_qty_side_down).toInt() == 1) {
            p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
            p.br();
            p.ltext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(1).toDouble(), 3))
                    .arg(data.at(i).at(2).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble(), 2)), 0);
            p.br();
            totalQty += data.at(i).at(1).toDouble();
        } else if(__c5config.getValue(param_shop_print_goods_qty_side_left).toInt() == 1) {
            p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
            p.rtext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(1).toDouble(), 3))
                    .arg(data.at(i).at(2).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble(), 2)));
            p.br();
            totalQty += data.at(i).at(1).toDouble();
        } else {
            p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0);
            p.rtext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(1).toDouble(), 3))
                    .arg(data.at(i).at(2).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble(), 2)));
            p.br();
            totalQty += data.at(i).at(1).toDouble();
        }

        p.br();
        p.line();
        p.br(2);
    }

    if(__c5config.fMainJson["print_total_qty_goods"].toBool()) {
        p.line(4);
        p.br();
        p.lrtext(tr("Total qty"), float_str(totalQty, 2));
        p.br();
        p.br();
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

    if(amountCash > 0.001) {
        if(cash > 0.001) {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(cash, 2));
            p.br();

            if(change > 0.001) {
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

    if(amountCard > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(amountCard, 2));
        p.br();
    }

    if(amountPrepaid > 0.001) {
        p.ltext(tr("Prepaid used"), 0);
        p.rtext(float_str(amountPrepaid, 2));
        p.br();
    }

    if(!comment.isEmpty()) {
        p.br();
        p.ltext(comment, 0);
        p.br();
    }

    if(!__c5config.getValue(param_static_qr_text).isEmpty()) {
        p.br();
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = __c5config.getValue(param_static_qr_text);
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                                  encodeString.toUtf8().data());

        if(!successfulEncoding) {
            //fLog.append("Cannot encode qr image");
        }

        int qrImageSize = qrEncode.m_nSymbleSize;
        int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
        encodeImage.fill(1);

        for(int i = 0; i < qrImageSize; i++) {
            for(int j = 0; j < qrImageSize; j++) {
                if(qrEncode.m_byModuleData[i][j]) {
                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(encodeImage);
        pix = pix.scaled(300, 300);
        p.image(pix, Qt::AlignHCenter);
        p.br();
        /* End QRCode */
    }

    p.br();
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(printer);
}

void PrintReceiptGroup::print2(const QString &id, C5Database &db)
{
    QMap<QString, QVariant> returnFrom;
    OHeader oh;
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");

    if(!oh.getRecord(db)) {
        throw std::exception(QString("No order for %1").arg(id).toStdString().data());
    }

    QJsonObject jtax;
    db[":f_order"] = id;
    db.exec("select * from o_tax_log where f_order=:f_order and f_state=1");

    if(db.nextRow()) {
        jtax = QJsonDocument::fromJson(db.getString("f_out").toUtf8()).object();
    }

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");

    if(!db.nextRow()) {
        return;
    }

    QString saletype;

    if(oh.amountTotal < 0) {
        saletype = tr("Return");
        db[":f_header"] = id;
        db.exec("select f_returnfrom from o_goods where f_header=:f_header");

        if(db.nextRow()) {
            db[":f_id"] = db.getString("f_returnfrom");
            db.exec("select f_header from o_goods where f_id=:f_id");
            db.nextRow();
            db[":f_id"] = db.getString("f_header");
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
            db.rowToMap(returnFrom);
            db[":f_id"] = id;
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
        }
    }

    db[":f_trsale"] = id;
    db.exec("select * from b_gift_card_history where f_trsale=:f_trsale");
    QMap<QString, QVariant> acc;

    if(db.nextRow()) {
        db.rowToMap(acc);
        db[":f_card"] = db.getInt("f_card");
        db.exec("select sum(f_amount) as f_totalamount from b_gift_card_history where f_card=:f_card");
        db.nextRow();
        db.rowToMap(acc);
    }

    CPartners partner;

    if(oh.partner > 0) {
        db[":f_id"] = oh.partner;
        db.exec("select * from c_partners where f_id=:f_id");

        if(!partner.getRecord(db)) {
            throw std::exception(QString("No partner with %1 code").arg(oh.partner).toStdString().data());
        }
    }

    QString price1, price2;
    int rw = 1;

    switch(rw) {
    case 1:
        price1 = "ad.f_price";
        price2 = "gpr1.f_price2";
        break;

    case 2:
        price1 = "gpr1.f_price1";
        price2 = "gpr1.f_price1";
        break;
    }

    //Print blocks
    double tt = 0;
    db[":f_header"] = id;
    db.exec(QString("select "
                    "if (gc.f_base is null, g2.f_scancode, g.f_scancode) as f_goods, "
                    "sum(if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty)) as f_qty, "
                    "if(gc.f_base is null, %1, %2) as f_price, "
                    "sum(if(gc.f_base is null, %1*ad.f_qty, %2*(gc.f_qty*ad.f_qty))) as f_total %3 "
                    "from o_goods ad "
                    "left join c_goods_complectation gc on gc.f_base=ad.f_goods "
                    "left join c_goods g on g.f_id=gc.f_goods "
                    "left join c_goods g2 on g2.f_id=ad.f_goods "
                    "inner join o_header oh on oh.f_id=ad.f_header "
                    "left join c_groups t1 on t1.f_id=g.f_group "
                    "left join c_groups t2 on t2.f_id=g2.f_group "
                    "left join c_goods_prices gpr1 on gpr1.f_goods=g.f_id "
                    "left join c_goods_prices gpr2 on gpr2.f_goods=g2.f_id "
                    "where oh.f_id=:f_header and g2.f_unit=10 "
                    "group by 1, 3 %4 "
                    "order by ad.f_row ")
            .arg(price1)
            .arg(price2)
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? "" : ",ad.f_id")
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? "" : ",5"));
    QList<QList<QVariant> > data;

    while(db.nextRow()) {
        QList<QVariant> v;

        for(int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }

        data.append(v);
        tt += db.getDouble("f_total");
    }

    switch(rw) {
    case 1:
        price1 = "ad.f_price";
        break;

    case 2:
        price1 = "gpr1.f_price1";
        break;
    }

    db[":f_header"] = id;
    QString goodsNameField = "g.f_name as f_goods";

    if(__c5config.getValue(param_print_scancode_with_name).toInt() == 1) {
        goodsNameField = "concat(g.f_name, ' ', g.f_scancode) as f_goods";
    }

    db.exec(QString("select %1, sum(ad.f_qty) as f_qty, %2,  sum(%2*ad.f_qty) as f_total %3 "
                    "from o_goods ad "
                    "left join c_goods g on g.f_id=ad.f_goods "
                    "inner join o_header oh on oh.f_id=ad.f_header "
                    "left join c_groups t1 on t1.f_id=g.f_group "
                    "where oh.f_id=:f_header and g.f_unit<>10 "
                    "group by 1, 3 %4 "
                    "order by ad.f_row ")
            .arg(goodsNameField)
            .arg(price1)
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? ",ad.f_discountamount" :
    ",ad.f_discountamount,ad.f_id")
            .arg(__c5config.getValue(param_shop_print_dontgroup).toInt() == 0 ? ",5" : ",5,6"));

    while(db.nextRow()) {
        QList<QVariant> v;

        for(int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }

        data.append(v);
        tt += db.getDouble("f_total");
    }

    const int bs = 12;
    QFont font(qApp->font());
    font.setPointSize(bs);
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(C5Config::localReceiptPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.br();
    p.br();
    p.setFont(font);
    p.br(2);

    if(!saletype.isEmpty()) {
        p.ctext(saletype);
        p.br();
    }

    if(jtax["rseq"].toInt() > 0) {
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(oh.dateCash.toString(FORMAT_DATE_TO_STR) + " " + oh.timeClose.toString(FORMAT_TIME_TO_SHORT_STR));
        p.br();
        p.ltext(tr("(F)"), 0);
    }

    if(partner.id.toInt() > 0) {
        p.ltext(tr("Buyer taxcode"), 0);
        p.rtext(partner.taxCode);
        p.br();
        p.ltext(partner.taxName, 0);
        p.br();
    }

    p.setFontBold(true);
    p.ctext(QString("#%1").arg(oh.humanId()));
    p.br();

    if(returnFrom.count() > 0) {
        p.ctext(QString("(%1 %2%3)").arg(tr("Return from")).arg(returnFrom["f_prefix"].toString()).arg(
                    returnFrom["f_hallid"].toInt()));
        p.br();
    }

    p.setFontSize(bs - 2);
    p.ltext(tr("Name"), 0, 31);
    p.ltext(tr("Qty"), 33, 10);
    p.ltext(tr("Price"), 41, 12);
    p.rtext(tr("Total"));
    p.setFontBold(false);
    p.br();
    p.line(1);
    p.br(3);

    for(int i = 0; i < data.count(); i++) {
        p.ltext(QString("%1").arg(data.at(i).at(0).toString()), 0, 31);
        p.ltext(float_str(data.at(i).at(1).toDouble(), 3), 33, 10);
        p.ltext(float_str(data.at(i).at(2).toDouble(), 2), 41, 12);
        p.rtext(float_str(data.at(i).at(3).toDouble(), 2));
        p.br();

        if(data.at(i).at(4).toDouble() > 0.001) {
            p.ltext(QString("%1%2 %3").arg(tr("Discount"), tr(":"), float_str(data.at(i).at(4).toDouble(), 2)));
            p.br();
        }

        p.line();
        p.br(2);
    }

    p.line(1);
    p.br(3);
    p.setFontBold(true);
    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(tt, 2));
    p.br();
    p.line();
    p.br();

    if(oh.amountCash > 0.001) {
        p.ltext(tr("Payment, cash"), 0);
        p.rtext(float_str(oh.amountCash, 2));
        p.br();

        if(oh.amountCashIn > 0.001) {
            p.ltext(tr("Cash in"), 0);
            p.rtext(float_str(oh.amountCashIn, 2));
            p.br();

            if(oh.amountChange > 0.001) {
                p.ltext(tr("Change"), 0);
                p.rtext(float_str(-1 * oh.amountChange, 2));
                p.br();
            }
        } else {
        }
    }

    if(oh.amountCard > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(oh.amountCard, 2));
        p.br();
    }

    if(oh.amountIdram > 0.001) {
        p.ltext(tr("Payment, idram"), 0);
        p.rtext(float_str(oh.amountIdram, 2));
        p.br();
    }

    if(oh.amountTelcell > 0.001) {
        p.ltext(tr("Payment, TellCell"), 0);
        p.rtext(float_str(oh.amountTelcell, 2));
        p.br();
    }

    if(oh.amountDiscount > 0.001) {
        p.ltext(tr("Discount"), 0);
        p.rtext(float_str(oh.amountDiscount, 2));
        p.br();
    }

    if(oh.amountPrepaid > 0.001) {
        p.ltext(tr("Prepaid"), 0);
        p.rtext(float_str(oh.amountPrepaid, 2));
        p.br();
    }

    if(oh.amountBank > 0.001) {
        p.ltext(tr("Bank transfer"), 2);
        p.rtext(float_str(oh.amountBank, 2));
        p.br();
    }

    if(oh.amountDebt > 0.001) {
        p.ltext(tr("Debt"), 2);
        p.rtext(float_str(oh.amountDebt, 2));
        p.br();
    }

    if(!oh.comment.isEmpty()) {
        p.br();
        p.ltext(oh.comment, 0);
        p.br();
    }

    if(!acc.isEmpty()) {
        p.br();
        p.lrtext(tr("Accumulated"), float_str(acc["f_amount"].toDouble(), 1));
        p.br();
        p.lrtext(tr("Accumulated amount"), float_str(acc["f_totalamount"].toDouble(), 1));
        p.br();
    }

    if(!__c5config.getValue(param_static_qr_text).isEmpty()) {
        p.br();
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = __c5config.getValue(param_static_qr_text);
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                                  encodeString.toUtf8().data());

        if(!successfulEncoding) {
            //fLog.append("Cannot encode qr image");
        }

        int qrImageSize = qrEncode.m_nSymbleSize;
        int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
        encodeImage.fill(1);

        for(int i = 0; i < qrImageSize; i++) {
            for(int j = 0; j < qrImageSize; j++) {
                if(qrEncode.m_byModuleData[i][j]) {
                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(encodeImage);
        pix = pix.scaled(300, 300);
        p.image(pix, Qt::AlignHCenter);
        p.br();
        /* End QRCode */
    }

    p.setFontBold(false);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(printer);
}

void PrintReceiptGroup::print3(const QString &id, C5Database &db)
{
    QMap<QString, QVariant> oheader;
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    db.rowToMap(oheader);
    QJsonObject jtax;
    db[":f_order"] = id;
    db.exec("select * from o_tax_log where f_order=:f_order and f_state=1");

    if(db.nextRow()) {
        jtax = QJsonDocument::fromJson(db.getString("f_out").toUtf8()).object();
    }

    QString saletype;
    QMap<QString, QVariant> returnFrom;

    if(oheader["f_amounttotal"].toDouble() < 0) {
        saletype = tr("Return");
        db[":f_header"] = id;
        db.exec("select f_returnfrom from o_goods where f_header=:f_header");

        if(db.nextRow()) {
            db[":f_id"] = db.getString("f_returnfrom");
            db.exec("select f_header from o_goods where f_id=:f_id");
            db.nextRow();
            db[":f_id"] = db.getString("f_header");
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
            db.rowToMap(returnFrom);
        }
    }

    int hallid = oheader["f_hallid"].toInt();
    QString pref = oheader["f_prefix"].toString();
    int partner = oheader["f_partner"].toInt();
    QString partnerName, partnerTaxcode;

    if(partner > 0) {
        db[":f_id"] = partner;
        db.exec("select * from c_partners where f_id=:f_id");

        if(db.nextRow()) {
            partnerTaxcode = db.getString("f_taxcode");
            partnerName = db.getString("f_taxname");
        }
    }

    QList<QMap<QString, QVariant> > ogoods;
    db[":f_header"] = id;
    db.exec("select g.f_scancode, g.f_name as f_goods, ad.f_qty, u.f_name as f_unit, ad.f_price, ad.f_total  "
            "from o_goods ad "
            "left join c_goods g on g.f_id=ad.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "inner join o_header oh on oh.f_id=ad.f_header "
            "where oh.f_id=:f_header "
            "order by ad.f_row ");

    while(db.nextRow()) {
        QMap<QString, QVariant> g;
        db.rowToMap(g);
        ogoods.append(g);
    }

    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(C5Config::localReceiptPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(font);
    p.br(2);

    if(!saletype.isEmpty()) {
        p.setFontSize(22);
        p.ctext(saletype);
        p.br();
        p.setFontSize(20);
    }

    if(jtax["rseq"].toInt() > 0) {
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        //        p.ltext(tr("Date"), 0);
        //        p.rtext(dtax.getString("f_time"));
        //        p.br();
        p.ltext(tr("(F)"), 0);
        p.br();
    }

    if(!partnerName.isEmpty() || !partnerTaxcode.isEmpty()) {
        p.ltext(tr("Buyer taxcode"), 0);
        p.rtext(partnerTaxcode);
        p.br();
        p.ltext(partnerName, 0);
        p.br();
    }

    p.setFontBold(true);
    p.ltext(QString("%1 #%2%3").arg(tr("Order"), pref, QString::number(hallid)), 0);
    p.rtext(QString("%1: %2").arg(tr("Date"), oheader["f_datecash"].toDate().toString(FORMAT_DATE_TO_STR)));
    p.br();

    if(returnFrom.count() > 0) {
        p.ctext(QString("(%1 %2%3)").arg(tr("Return from"), returnFrom["f_prefix"].toString()).arg(
                    returnFrom["f_hallid"].toInt()));
        p.br();
    }

    p.setFontSize(20);
    p.br(3);
    p.br();
    QList<qreal> points;
    QStringList vals;
    points << 10 << 100 << 200 << 600 << 250 << 250 << 250 << 250;
    vals.clear();
    vals << tr("NN")
         << tr("Code")
         << tr("Name")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    //TODO
    //p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    for(int i = 0; i < ogoods.count(); i++) {
        QMap<QString, QVariant>& row = ogoods[i];
        vals.clear();
        vals << QString::number(i + 1)
             << row["f_scancode"].toString()
             << row["f_goods"].toString()
             << float_str(row["f_qty"].toDouble(), 3)
             << row["f_unit"].toString()
             << float_str(row["f_price"].toDouble(), 2)
             << float_str(row["f_total"].toDouble(), 2);
        //TODO
        //p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }

    p.br();
    p.br();
    p.line(4);
    p.br();
    p.setFontBold(true);
    QString pay = tr("Payment");
    //    if (debt) {
    //        pay += ", " + tr("debt");
    //    }
    p.ltext(pay, 0);
    p.br();

    if(oheader["f_amountcash"].toDouble() > 0.001) {
        p.ltext(tr("Payment, cash"), 0);
        p.ltext(float_str(oheader["f_amounttotal"].toDouble(), 2), 200);
        p.br();

        if(oheader["f_cash"].toDouble() > 0.001) {
            p.ltext(tr("Cash"), 0);
            p.rtext(float_str(oheader["f_cash"].toDouble(), 2));
            p.br();

            if(oheader["f_change"].toDouble() > 0.001) {
                p.ltext(tr("Change"), 0);
                p.rtext(float_str(-1 * oheader["f_change"].toDouble(), 2));
                p.br();
            }
        }
    }

    if(oheader["f_amountcard"].toDouble() > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.ltext(float_str(oheader["f_amountcard"].toDouble(), 2), 200);
        p.br();
    }

    if(!oheader["f_comment"].toString().isEmpty()) {
        p.br();
        p.ltext(oheader["f_comment"].toString(), 0);
        p.br();
    }

    if(!__c5config.getValue(param_static_qr_text).isEmpty()) {
        p.br();
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = __c5config.getValue(param_static_qr_text);
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                                  encodeString.toUtf8().data());

        if(!successfulEncoding) {
            //fLog.append("Cannot encode qr image");
        }

        int qrImageSize = qrEncode.m_nSymbleSize;
        int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
        encodeImage.fill(1);

        for(int i = 0; i < qrImageSize; i++) {
            for(int j = 0; j < qrImageSize; j++) {
                if(qrEncode.m_byModuleData[i][j]) {
                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(encodeImage);
        pix = pix.scaled(300, 300);
        p.image(pix, Qt::AlignHCenter);
        p.br();
        /* End QRCode */
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br();
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(printer);
}
