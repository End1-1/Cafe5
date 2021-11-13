#include "c5printreceiptthread.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5translator.h"
#include "QRCodeGenerator.h"
#include "datadriver.h"
#include "c5config.h"
#include <QApplication>

C5PrintReceiptThread::C5PrintReceiptThread(const QString &header, const QMap<QString, QVariant> &headerInfo, const QList<QMap<QString, QVariant> > &bodyinfo, const QString &printer, int language, int paperWidth, QObject *parent) :
    QObject(parent)
{
    fHeader = header;
    fPrinter = printer;
    fPaperWidth = paperWidth;
    fLanguage = language;
    fBill = false;
    fHeaderInfo = headerInfo;
    fBodyInfo = bodyinfo;
}

bool C5PrintReceiptThread::print()
{
    C5Translator __translator;
    __translator.initTranslator(__c5config.dbParams());
    __translator.setLanguage(fLanguage);
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(fPaperWidth, 2800, QPrinter::Portrait);
    p.setFont(font);

    QMap<QString, QVariant> options;
    QMap<QString, QVariant> taxinfo;
    QMap<QString, QVariant> clinfo;
    QMap<QString, QVariant> roominfo;
    QMap<QString, QVariant> carinfo;
    C5Database db(__c5config.dbParams());

    db[":f_id"] = fHeader;
    if (!db.exec("select * from o_header_options where f_id=:f_id")) {
        fError = db.fLastError;
        return false;
    }
    if (!db.nextRow()) {
        fError = db.fLastError;
        return false;
    }
    db.rowToMap(options);

    db[":f_id"] = fHeader;
    if (!db.exec("select * from o_tax where f_id=:f_id")) {
        fError = db.fLastError;
        return false;
    }
    if (!db.nextRow()) {
        fError = tr("Missing tax record");
        return false;
    }
    db.rowToMap(taxinfo);

    db[":f_id"] = fHeader;
    if (!db.exec("select * from o_pay_cl where f_id=:f_id")) {
        fError = db.fLastError;
        return false;
    }
    if (!db.nextRow()) {
        fError = db.fLastError;
        return false;
    }
    db.rowToMap(clinfo);

    db[":f_id"] = fHeader;
    if (!db.exec("select * from o_pay_room where f_id=:f_id")) {
        fError = db.fLastError;
        return false;
    }
    if (!db.nextRow()) {
        fError = db.fLastError;
        return false;
    }
    db.rowToMap(roominfo);

    db[":f_id"] = options["f_car"].toInt();
    if (!db.exec("select * from b_car where f_id=:f_id")) {
        fError = db.fLastError;
        return false;
    }
    if (db.nextRow()) {
        db.rowToMap(carinfo);
    }

    if (fBill) {
        p.image("./logo_bill.png", Qt::AlignHCenter);
    } else {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
    }
    p.br();
    p.setFontBold(true);
    p.ctext(__translator.tt(tr("Receipt #")) + QString("%1%2").arg(fHeaderInfo["f_prefix"].toString()).arg(fHeaderInfo["f_hallid"].toString()));
    p.br();
    if (fHeaderInfo["f_otherid"].toInt() == PAYOTHER_SELFCOST) {
        p.setFontSize(24);
        p.setFontBold(true);
        p.ctext(__translator.tt(tr("Breakfast")));
        p.br();
        p.br(4);
        p.setFontSize(20);
        p.setFontBold(false);
    }
    p.setFontBold(false);
    if (taxinfo["f_receiptnumber"].toInt() > 0) {
        p.ltext(taxinfo["f_firmname"].toString(), 0);
        p.br();
        p.ltext(taxinfo["f_address"].toString(), 0);
        p.br();
        p.ltext(__translator.tt(tr("Department")), 0);
        p.rtext(taxinfo["f_dept"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Tax number")), 0);
        p.rtext(taxinfo["f_hvhh"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Device number")), 0);
        p.rtext(taxinfo["f_devnum"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Serial")), 0);
        p.rtext(taxinfo["f_serial"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Fiscal")), 0);
        p.rtext(taxinfo["f_fiscal"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Receipt number")), 0);
        p.rtext(taxinfo["f_receiptnumber"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Date")), 0);
        p.rtext(taxinfo["f_taxtime"].toString());
        p.br();
        p.ltext(__translator.tt(tr("(F)")), 0);
        p.br();
    }
    p.br(1);
    p.ltext(__translator.tt(tr("Table")), 0);
    p.rtext(QString("%1/%2").arg(dbhall->name(fHeaderInfo["f_hall"].toInt())).arg(dbtable->name(fHeaderInfo["f_table"].toInt())));
    p.br();
    p.ltext(__translator.tt(tr("Printed")), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(__translator.tt(tr("Staff")), 0);
    p.rtext(dbuser->fullName(fHeaderInfo["f_currentstaff"].toInt()));
    p.br();
    p.br(2);
    p.line();
    p.br(2);
    p.ctext(__translator.tt(tr("Class | Name | Qty | Price | Total")));
    p.br();
    p.br(4);
    p.line();
    p.br(1);

    bool noservice = false, nodiscount = false;
    for (int i = 0; i < fBodyInfo.count(); i++) {
        const QMap<QString, QVariant> &m = fBodyInfo.at(i);
        if (m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        QString name;
        if (!m["f_adgcode"].toString().isEmpty()) {
            name += QString("%1: %2").arg(__translator.tt(tr("Class"))).arg(m["f_adgcode"].toString());
        }
        name += __translator.td(m["f_dish"].toInt());
        if (m["f_canservice"].toInt() == 0) {
            noservice = true;
            name += "*";
        }
        if (m["f_candiscount"].toInt() == 0) {
            nodiscount = true;
            name += "**";
        }
        p.ltext(name, 0);
        p.br();
        QString totalStr = m["f_total"].toDouble() > 0.001 ? float_str(m["f_total"].toDouble(), 2) : tr("Present");
        if (str_float(totalStr) < 0.001) {
            p.rtext(totalStr);
        } else {
            if (dbdish->isHourlyPayment(m["f_dish"].toInt())) {
                p.rtext(QString("%1 = %2").arg(m["f_comment"].toString()).arg(float_str(m["f_total"].toDouble(), 2)));
            } else {
                p.rtext(QString("%1 x %2 = %3")
                        .arg(float_str(m["f_qty2"].toDouble(), 2))
                        .arg(float_str(m["f_price"].toDouble(), 2))
                        .arg(totalStr));
            }
        }
        if (dbdish->isExtra(m["f_dish"].toInt())) {
            p.br();
            p.ltext(m["f_comment"].toString(), 0);
        }
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }
    p.br();
    p.ltext(__translator.tt(tr("Total")), 0);
    p.rtext(float_str(fHeaderInfo["f_amounttotal"].toDouble() + fHeaderInfo["f_amountdiscount"].toDouble() - fHeaderInfo["f_amountservice"].toDouble(), 2));
    p.br();
    p.br(1);
    if (fHeaderInfo["f_amountservice"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Service"))).arg(fHeaderInfo["f_servicefactor"].toDouble() * 100), 0);
        p.rtext(float_str(fHeaderInfo["f_amountservice"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    if (fHeaderInfo["f_discountfactor"].toDouble() > 0.01) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Discount"))).arg(float_str(fHeaderInfo["f_discountfactor"].toDouble() * 100, 2)), 0);
        p.rtext(float_str(fHeaderInfo["f_amountdiscount"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    p.setFontSize(24);
    p.setFontBold(true);
    p.ltext(__translator.tt(tr("Need to pay")), 0);
    p.setFontSize(28);
    p.rtext(float_str(fHeaderInfo["f_amounttotal"].toDouble(), 2));
    p.br();
    p.br();
    p.line();
    p.setFontSize(20);
    p.setFontBold(false);
    if (noservice) {
        p.ltext(QString("* - %1").arg("No service"), 0);
        p.br();
    }
    if (nodiscount) {
        p.ltext(QString("** - %1").arg("No discount"), 0);
        p.br();
    }
    p.br();
    p.line();
    /*
     TODO: uncomment this
    if (fHeader["f_idramid"].toString().length() > 0) {
        p.setFontBold(false);
        p.ctext(__translator.tt(tr("Pay by IDRAM")));
        p.br();
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = QString("%1;%2;%3;%4|%5;%6;")
                .arg("Jazzve")
                .arg(fHeader["f_idramid"].toString()) //IDram ID
                .arg(fHeader["f_amounttotal"].toString())
                .arg(QString("%1-%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_id"].toString()))
                .arg(fHeader["f_idramphone"].toString())
                .arg("1");
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
        if (successfulEncoding) {
            int qrImageSize = qrEncode.m_nSymbleSize;
            int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
            QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
            encodeImage.fill(1);

            for ( int i = 0; i < qrImageSize; i++ )
                for ( int j = 0; j < qrImageSize; j++ )
                    if ( qrEncode.m_byModuleData[i][j] )
                        encodeImage.setPixel( i + QR_MARGIN, j + QR_MARGIN, 0 );

            QPixmap pix = QPixmap::fromImage( encodeImage );
            pix = pix.scaled(300, 300);
            p.image(pix, Qt::AlignHCenter);
            p.br();
        }
    }
    */

    if (!fBill) {
        if (fHeaderInfo["f_amountcash"].toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Payment, cash")), 0);
            p.rtext(float_str(fHeaderInfo["f_amountcash"].toDouble(), 2));
        }
        if (fHeaderInfo["f_amountcard"].toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Payment, card")), 0);
            p.rtext(float_str(fHeaderInfo["f_amountcard"].toDouble(), 2));
        }
        if (fHeaderInfo["f_amountbank"].toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Bank transfer")), 0);
            p.rtext(float_str(fHeaderInfo["f_amountbank"].toDouble(), 2));
        }
        p.br();
    }

    p.setFontSize(24);
    if (!roominfo["f_res"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt(tr("Transfer to room")));
        p.br();
        p.ctext(roominfo["f_room"].toString() + ", " + roominfo["f_guest"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (!clinfo["f_code"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt(tr("City ledger")));
        p.br();
        p.ctext(clinfo["f_code"].toString() + ", " + clinfo["f_name"].toString());
        p.br(p.fLineHeight * 3);
    }

    if (fHeaderInfo["f_otherid"].toInt() == PAYOTHER_DEBT) {
        p.br();
        p.ctext(__translator.tt(tr("Debt")) + " " + clinfo["f_name"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (fHeaderInfo["f_otherid"].toInt() == PAYOTHER_COMPLIMENTARY) {
        p.br();
        p.ctext(__translator.tt(tr("Complimentary")) + " " + fHeaderInfo["f_comment"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (options["f_car"].toInt() > 0) {
        p.br();
        p.ltext(tr("Costumer"), 0);
        p.br();
        p.ltext(dbpartner->contact(carinfo["f_costumer"].toInt()), 0);
        p.br();
        p.ltext(dbcar->name(carinfo["f_car"].toInt()), 0);
        p.br();
        p.ltext(carinfo["f_govnumber"].toString(), 0);
        p.br(p.fLineHeight * 2);
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.ltext(__translator.tt(tr("Thank you for visit!")), 0);
    p.br();
    if (fBill) {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(abs(fHeaderInfo["f_precheck"].toInt()) + 1), 0);
    } else {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(abs(fHeaderInfo["f_print"].toInt()) + 1), 0);
    }
    p.br();

    if (!fHeaderInfo["f_comment"].toString().isEmpty()) {
        p.br();
        p.br();
        p.br();
        p.br();
        p.br();
        p.line();
        p.br();
        p.ltext(fHeaderInfo["f_comment"].toString(), 0);
        p.br();
    }
    p.print(fPrinter, QPrinter::Custom);

    return true;
}
