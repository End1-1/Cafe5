#include "c5printreceiptthread.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5translator.h"
#include "QRCodeGenerator.h"
#include <QApplication>

C5PrintReceiptThread::C5PrintReceiptThread(const QStringList &dbParams, const QJsonObject &header, const QJsonArray &body, const QString &printer, QObject *parent) :
    QThread(parent)
{
    fHeader = header;
    fBody = body;
    fPrinter = printer;
    fDbParams = dbParams;
    fBill = false;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(startPrint()), this, SLOT(print()));
}

void C5PrintReceiptThread::run()
{
    emit startPrint();
}

void C5PrintReceiptThread::print()
{
    C5Translator __translator;
    __translator.initTranslator(fDbParams);
    __translator.setLanguage(fHeader["f_receiptlanguage"].toString().toInt());
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.setFontBold(true);
    p.ctext(__translator.tt(tr("Receipt #")) + QString("%1%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_hallid"].toString()));
    p.br();
    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_SELFCOST) {
        p.setFontSize(24);
        p.setFontBold(true);
        p.ctext(__translator.tt(tr("Breakfast")));
        p.br();
        p.br(4);
        p.setFontSize(20);
        p.setFontBold(false);
    }
    p.setFontBold(false);
    if (fHeader["f_printtax"].toString().toInt() && !fBill) {
        p.ltext(fHeader["f_firmname"].toString(), 0);
        p.br();
        p.ltext(fHeader["f_address"].toString(), 0);
        p.br();
        p.ltext(__translator.tt(tr("Department")), 0);
        p.rtext(fHeader["f_dept"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Tax number")), 0);
        p.rtext(fHeader["f_hvhh"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Device number")), 0);
        p.rtext(fHeader["f_devnum"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Serial")), 0);
        p.rtext(fHeader["f_serial"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Fiscal")), 0);
        p.rtext(fHeader["f_fiscal"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Receipt number")), 0);
        p.rtext(fHeader["f_receiptnumber"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Date")), 0);
        p.rtext(fHeader["f_taxtime"].toString());
        p.br();
        p.ltext(__translator.tt(tr("(F)")), 0);
        p.br();
    }
    p.br(1);
    p.ltext(__translator.tt(tr("Table")), 0);
    p.rtext(QString("%1/%2").arg(fHeader["f_hallname"].toString()).arg(fHeader["f_tablename"].toString()));
    p.br();
    p.ltext(__translator.tt(tr("Printed")), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(__translator.tt(tr("Staff")), 0);
    p.rtext(fHeader["f_currentstaffname"].toString());
    p.br();
    p.br(2);
    p.line();
    p.br(2);
    p.ctext(__translator.tt(tr("Class | Name | Qty | Price | Total")));
    p.br();
    p.br(4);
    p.line();
    p.br(1);
    int nn = 1;
    double total = 0;
    for (int i = 0; i < fBody.count(); i++) {
        QJsonObject o = fBody.at(i).toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        total += o["f_qty2"].toString().toDouble() * o["f_price"].toString().toDouble();
        QString name = QString("%1.").arg(nn++);
        if (!o["f_adgcode"].toString().isEmpty()) {
            name += QString("%1: %2").arg(__translator.tt(tr("Class"))).arg(o["f_adgcode"].toString());
        }
        name += o["f_name"].toString();
        p.ltext(name, 0);
        p.br();
//        p.ltext(o["f_name"].toString(), 0);
//        p.br(); <--- this row fuck
        QString servPlus;
        QString servValue = float_str(fHeader["f_servicefactor"].toString().toDouble() * 100, 2) + "% ";
        QString totalStr = str_float(o["f_total"].toString()) > 0.001 ? float_str(str_float(o["f_total"].toString()), 2) : tr("Present");
        if (fHeader["f_servicemode"].toString().toInt() == SERVICE_AMOUNT_MODE_INCREASE_PRICE) {
            if (str_float(fHeader["f_servicefactor"].toString()) > 0.001) {
                servPlus = "+";
            } else {
                servValue = "";
            }
            if (str_float(totalStr) < 0.001) {
                p.rtext(totalStr);
            } else {
                p.ltext(QString("%1 x %2 %3 %4 = %5")
                        .arg(float_str(o["f_qty2"].toString().toDouble(), 2))
                        .arg(float_str(o["f_price"].toString().toDouble(), 2))
                        .arg(servPlus).arg(servValue)
                        .arg(total), 0);
            }
        } else {
            if (str_float(totalStr) < 0.001) {
                p.rtext(totalStr);
            } else {
                p.rtext(QString("%1 x %2 = %5")
                        .arg(float_str(o["f_qty2"].toString().toDouble(), 2))
                        .arg(float_str(o["f_price"].toString().toDouble(), 2))
                        .arg(totalStr));
            }
        }
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }
    p.br();
    switch (fHeader["f_servicemode"].toString().toInt()) {
    case SERVICE_AMOUNT_MODE_INCREASE_PRICE:
        break;
    case SERVICE_AMOUNT_MODE_SEPARATE:
        if (str_float(fHeader["f_servicefactor"].toString()) > 0.0001) {
            p.br(2);
            p.line();
            p.ltext(QString("%1").arg(__translator.tt(tr("Total"))), 0);
            p.rtext(float_str(total, 2));
            p.br();
            p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Service"))).arg(float_str(str_float(fHeader["f_servicefactor"].toString()) * 100, 2)), 0);
            p.rtext(float_str(str_float(fHeader["f_amountservice"].toString()), 2));
            p.br();
            p.br(2);
            p.line();
            p.br(1);
        }
        break;
    }
    p.ltext(__translator.tt(tr("Grand total")), 0);
    p.rtext(float_str(str_float(fHeader["f_amounttotal"].toString()), 2));
    p.br();
    if (fHeader["f_servicemode"].toString().toInt() == SERVICE_AMOUNT_MODE_INCREASE_PRICE && str_float(fHeader["f_servicefactor"].toString()) > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Service included"))).arg(float_str(str_float(fHeader["f_servicefactor"].toString()) * 100, 2)), 0);
        p.rtext(float_str(fHeader["f_amountservice"].toString().toDouble(), 2));
        p.br();
    }
    if (str_float(fHeader["f_discountfactor"].toString()) > 0.01) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Discount included"))).arg(float_str(str_float(fHeader["f_discountfactor"].toString()) * 100, 2)), 0);
        p.rtext(float_str(str_float(fHeader["f_amountdiscount"].toString()), 2));
        p.br();
    }
    p.setFontBold(true);
    p.ltext(__translator.tt(tr("Need to pay")), 0);
    p.rtext(float_str(str_float(fHeader["f_amounttotal"].toString()), 2));

    p.line();
    p.br();
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

    if (!fBill) {
        if (str_float(fHeader["f_amountcash"].toString()) > 0.001) {
            p.ltext(__translator.tt(tr("Payment, cash")), 0);
            p.rtext(float_str(str_float(fHeader["f_amountcash"].toString()), 2));
        }
        if (str_float(fHeader["f_amountcard"].toString()) > 0.001) {
            p.ltext(__translator.tt(tr("Payment, card")), 0);
            p.rtext(float_str(str_float(fHeader["f_amountcard"].toString()), 2));
        }
        if (str_float(fHeader["f_amountbank"].toString()) > 0.001) {
            p.ltext(__translator.tt(tr("Bank transfer")), 0);
            p.rtext(float_str(str_float(fHeader["f_amountbank"].toString()), 2));
        }
        p.br();
    }

    p.setFontSize(24);
    if (!fHeader["f_other_room"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt(tr("Transfer to room")));
        p.br();
        p.ctext(fHeader["f_other_room"].toString() + ", " + fHeader["f_other_guest"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (!fHeader["f_other_clcode"].toString().isEmpty() && fHeader["f_otherid"].toString().toInt() == PAYOTHER_CL) {
        p.br();
        p.ctext(__translator.tt(tr("City ledger")));
        p.br();
        p.ctext(fHeader["f_other_clcode"].toString() + ", " + fHeader["f_other_clname"].toString());
        p.br(p.fLineHeight * 3);
    }

    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_DEBT) {
        p.br();
        p.ctext(__translator.tt(tr("Debt")) + " " + fHeader["f_other_clname"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_COMPLIMENTARY) {
        p.br();
        p.ctext(__translator.tt(tr("Complimentary")) + " " + fHeader["f_comment"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (fHeader["car"].toString().toInt() > 0) {
        p.br();
        p.ltext(tr("Costumer"), 0);
        p.br();
        p.ltext(fHeader["car_costumer"].toString(), 0);
        p.br();
        p.ltext(fHeader["car_model"].toString(), 0);
        p.br();
        p.ltext(fHeader["car_govnumber"].toString(), 0);
        p.br(p.fLineHeight * 2);
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.ltext(__translator.tt(tr("Thank you for visit!")), 0);
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(fHeader["f_print"].toString()), 0);
    p.br();
    p.print(fPrinter, QPrinter::Custom);
}
