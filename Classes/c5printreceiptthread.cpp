#include "c5printreceiptthread.h"
#include "c5printing.h"
#include "c5utils.h"
#include "QRCodeGenerator.h"
#include <QApplication>

C5PrintReceiptThread::C5PrintReceiptThread(const QJsonObject &header, const QJsonArray &body, const QString &printer, QObject *parent) :
    QThread(parent)
{
    fHeader = header;
    fBody = body;
    fPrinter = printer;
}

void C5PrintReceiptThread::run()
{
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.ltext(fHeader["f_firmname"].toString(), 0);
    p.br();
    p.ltext(fHeader["f_address"].toString(), 0);
    p.br();
    p.ltext(tr("Department"), 0);
    p.rtext(fHeader["f_dept"].toString());
    p.br();
    p.ltext(tr("Tax number"), 0);
    p.rtext(fHeader["f_hvhh"].toString());
    p.br();
    p.ltext(tr("Device number"), 0);
    p.rtext(fHeader["f_devnum"].toString());
    p.br();
    p.ltext(tr("Serial"), 0);
    p.rtext(fHeader["f_serial"].toString());
    p.br();
    p.ltext(tr("Fiscal"), 0);
    p.rtext(fHeader["f_fiscal"].toString());
    p.br();
    p.ltext(tr("Receipt number"), 0);
    p.rtext(fHeader["f_receiptnumber"].toString());
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(fHeader["f_taxtime"].toString());
    p.br();
    p.ltext(tr("(F)"), 0);
    p.br();
    p.line();
#ifdef QT_DEBUG
    p.br();
    p.line(0, p.fTop, 350, p.fTop);
#endif
    p.br(1);
    p.ltext(tr("Order"), 0);
    p.rtext(QString("%1%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_id"].toString()));
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(QString("%1/%2").arg(fHeader["f_hallname"].toString()).arg(fHeader["f_tablename"].toString()));
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(fHeader["f_currentstaffname"].toString());
    p.br();
    p.br(1);
    p.line();
    p.br(1);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    p.br();
    p.br(2);
    p.line();
    p.br(1);
    int nn = 1;
    for (int i = 0; i < fBody.count(); i++) {
        QJsonObject o = fBody.at(i).toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        p.ltext(QString("%1. %2: %3").arg(nn++).arg(tr("Class")).arg(o["f_adgcode"].toString()), 0);
        p.br();
        p.ltext(o["f_name"].toString(), 0);
        p.br();
        p.ltext(QString("%1 x %2 %3 %4 = %5").arg(float_str(o["f_qty2"].toString().toDouble(), 2))
                .arg(float_str(o["f_price"].toString().toDouble(), 2))
                .arg("").arg("")
                .arg(float_str(o["f_total"].toString().toDouble(), 2)), 0);
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }
    p.br();
    p.ltext(tr("Total"), 0);
    p.rtext(float_str(fHeader["f_amounttotal"].toString().toDouble(), 2));
    p.br();
    if (fHeader["f_servicefactor"].toString().toDouble() > 0.01) {
        p.ltext(QString("%1 %2%").arg(tr("Service included")).arg(float_str(fHeader["f_servicefactor"].toString().toDouble(), 2)), 0);
        p.rtext(float_str(fHeader["f_amountservice"].toString().toDouble(), 2));
        p.br();
    }
    if (fHeader["f_discountfactor"].toString().toDouble() > 0.01) {
        p.ltext(QString("%1 %2%").arg(tr("Service included")).arg(float_str(fHeader["f_discountfactor"].toString().toDouble(), 2)), 0);
        p.rtext(float_str(fHeader["f_amountdiscount"].toString().toDouble(), 2));
        p.br();
    }
    p.setFontBold(true);
    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(fHeader["f_amounttotal"].toString().toDouble(), 2));
    p.br();
    p.br();

    p.line();
    p.br();
    if (fHeader["f_idramid"].toString().length() > 0) {
        p.setFontBold(false);
        p.ctext(tr("Pay by IDRAM"));
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

    p.setFontBold(true);
    p.ltext(tr("Thank you for visit!"), 0);
    p.rtext(fHeader["f_print"].toString());
    p.br();
    p.print(fPrinter, QPrinter::Custom);
}
