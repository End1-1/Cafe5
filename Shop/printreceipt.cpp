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
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    QString hallid = db.getString("f_hallid");
    QString pref = db.getString("f_prefix");
    double amountTotal = db.getDouble("f_amounttotal");
    double amountCash = db.getDouble("f_amountcash");
    double amountCard = db.getDouble("f_amountcard");
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
    db.exec("select g.f_name, t.f_adgcode, g.f_scancode, o.f_qty, o.f_price, o.f_total, o.f_sign, o.f_taxdept "
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
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
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
        p.setFontBold(true);
        if (fModeRefund) {
            p.setFontSize(30);
            p.ctext(tr("Refund"));
            p.br();
        }
        p.ctext(QString("#%1%2").arg(pref).arg(hallid));
        p.br();
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
            p.ltext(QString("%1 X %2 = %3")
                    .arg(float_str(data.at(i).at(3).toDouble(), 2))
                    .arg(data.at(i).at(4).toDouble(), 2)
                    .arg(float_str(data.at(i).at(3).toDouble() * data.at(i).at(4).toDouble(), 2)), 0);
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
        p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    }
}
