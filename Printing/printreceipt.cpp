#include "printreceipt.h"
#include "c5printing.h"
#include "c5database.h"
#include "c5config.h"
#include "c5utils.h"
#include <QApplication>

PrintReceipt::PrintReceipt(QObject *parent) : QObject(parent)
{
}

void PrintReceipt::print(const QString &id, C5Database &db)
{
    QMap<QString, QVariant> returnFrom;
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    QString saletype;
    if (db.getDouble("f_amounttotal") < 0) {
        saletype = tr("Return");
        db[":f_header"] = id;
        db.exec("select f_returnfrom from o_goods where f_header=:f_header");
        if (db.nextRow()) {
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
    int hallid = db.getInt("f_hallid");
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
    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");
    bool tax = db.nextRow();
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    if (tax) {
        sn = db.getString("f_serial");
        firm = db.getString("f_firmname");
        address = db.getString("f_address");
        fiscal = db.getString("f_fiscal");
        hvhh = db.getString("f_hvhh");
        rseq = db.getString("f_receiptnumber");
        devnum = db.getString("f_devnum");
        time = db.getString("f_time");
    }
    db[":f_header"] = id;
    db.exec("select g.f_name, t.f_adgcode, g.f_scancode, o.f_qty, o.f_price, "
            "o.f_total, o.f_sign, o.f_taxdept, o.f_id "
            "from o_goods o "
            "left join c_goods g on g.f_id=o.f_goods "
            "left join c_groups t on t.f_id=g.f_group "
            "where o.f_header=:f_header "
            "order by o.f_row ");
    QList<QList<QVariant> > data;
    bool fModeRefund = false;
    QString taxDept;
    while (db.nextRow()) {
        QList<QVariant> v;
        if (db.getInt("f_sign") < 0) {
            fModeRefund = true;
        }
        taxDept = db.getString("f_taxdept");
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }
        data.append(v);
    }
    db[":f_header"] = id;
    db.exec("select og.f_id, g.f_name, g.f_scancode, gc.f_qty "
            "from c_goods_complectation gc "
            "inner join c_goods g on gc.f_goods=g.f_id "
            "inner join o_goods og on og.f_goods=gc.f_base "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_header");
    QMap<QString, QList<QList<QVariant> > > complectation;
    while (db.nextRow()) {
        QList<QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v.append(db.getValue(i));
        }
        complectation[db.getString("f_id")].append(v);
    }
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing p;
        p.setSceneParams(650, 2800, QPageLayout::Portrait);
        p.setFont(font);
        if (tax) {
            p.ltext(firm, 0);
            p.br();
            p.ltext(address, 0);
            p.br();
            p.ltext(tr("Department"), 0);
            p.rtext(taxDept);
            p.br();
            p.ltext(tr("Tax number"), 0);
            p.rtext(hvhh);
            p.br();
            p.ltext(tr("Device number"), 0);
            p.rtext(devnum);
            p.br();
            p.ltext(tr("Serial"), 0);
            p.rtext(sn);
            p.br();
            p.ltext(tr("Fiscal"), 0);
            p.rtext(fiscal);
            p.br();
            p.ltext(tr("Receipt number"), 0);
            p.rtext(rseq);
            p.br();
            p.ltext(tr("Date"), 0);
            p.rtext(time);
            p.br();
            p.ltext(tr("(F)"), 0);
            p.br();
        }
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
        if (fModeRefund) {
            p.setFontSize(30);
            p.ctext(tr("Refund"));
            p.br();
        }
        p.ctext(QString("#%1%2").arg(pref).arg(hallid));
        p.br();
        if (returnFrom.count() > 0) {
            p.ctext(QString("(%1 %2%3)").arg(tr("Return from"))
                    .arg(returnFrom["f_prefix"].toString())
                    .arg(returnFrom["f_hallid"].toInt()));
            p.br();
        }
        p.setFontSize(20);
        p.ctext(tr("Class | Name | Qty | Price | Total"));
        p.setFontBold(false);
        p.br();
        p.line(3);
        p.br(3);
        for (int i = 0; i < data.count(); i++) {
            if (tax) {
                p.ltext(QString("%1 %2, %3").arg(tr("Class:"))
                        .arg(data.at(i).at(1).toString())
                        .arg(data.at(i).at(0).toString()), 0);
            } else {
                p.ltext(QString("%1 %2").arg(data.at(i).at(2).toString()).arg(data.at(i).at(0).toString()), 0);
            }
            p.br();
            if (amountTotal > 0) {
                if (complectation.contains(data.at(i).at(8).toString())) {
                    QList<QList<QVariant> > rows = complectation[data.at(i).at(8).toString()];
                    for (int ri = 0; ri < rows.count(); ri++) {
                        QList<QVariant> rv = rows.at(ri);
                        p.ltext(QString("|---%1 %2 X %3").arg(rv.at(1).toString()).arg(rv.at(2).toString()).arg(float_str(rv.at(
                                    3).toDouble() *data.at(i).at(3).toDouble(), 2)), 0);
                        p.br();
                    }
                }
            }
            p.ltext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(3).toDouble(), 2))
                    .arg(data.at(i).at(4).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble() *data.at(i).at(4).toDouble(), 2)), 0);
            p.br();
            p.line();
            p.br(2);
        }
        p.line(4);
        p.br(3);
        p.setFontBold(true);
        if (fModeRefund) {
            p.ltext(tr("Total"), 0);
            p.rtext(float_str(amountTotal, 2));
            p.br();
        } else {
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
        }
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
        p.br();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QPageSize ps(QPageSize::Custom);
        p.print(C5Config::localReceiptPrinter(), ps);
#else
        p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
#endif
    }
}
