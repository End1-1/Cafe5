#include "c5printreceiptthread.h"
#include "c5printing.h"
#include "c5translator.h"
#include "QRCodeGenerator.h"
#include "c5utils.h"
#include <QApplication>
#include <QSettings>
#include <QJsonObject>

C5PrintReceiptThread::C5PrintReceiptThread(const QString &header, const QMap<QString, QVariant>& headerInfo,
        const QList<QMap<QString, QVariant> >& bodyinfo, const QString &printer, int language, int paperWidth)
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
    int bs = 24;
    C5Translator::initTranslator();
    C5Translator __translator;
    __translator.initTranslator();
    __translator.setLanguage(fLanguage);
    QFont font(qApp->font());

    if(fLanguage == LANG_RU) {
        font.setFamily("Ariac");
    }

    font.setPointSize(bs);
    C5Printing p;
    p.setSceneParams(fPaperWidth, 2600, QPageLayout::Portrait);
    p.setFont(font);
    QMap<QString, QVariant> options;
    QJsonObject jtax;
    QMap<QString, QVariant> clinfo;
    QMap<QString, QVariant> roominfo;
    QMap<QString, QVariant> preorder;
    QMap<QString, QVariant> carinfo;
    QMap<QString, QVariant> giftcardinfo;
    // C5Database db;
    // db[":f_id"] = fHeader;

    // if(!db.exec("select * from o_header_options where f_id=:f_id")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(!db.nextRow()) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // db.rowToMap(options);
    // db[":f_id"] = fHeader;

    // if(!db.exec("select * from o_preorder where f_id=:f_id")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(!db.nextRow()) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // db.rowToMap(preorder);
    // QMap<QString, QVariant> guestInfo;

    // if(fHeaderInfo["f_partner"].toInt() > 0) {
    //     db[":f_id"] = fHeaderInfo["f_partner"];
    //     db.exec("select * from c_partners where f_id=:f_id");

    //     if(db.nextRow()) {
    //         db.rowToMap(guestInfo);
    //     }
    // }

    // db[":f_id"] = fHeader;

    // if(!db.exec("select * from o_tax_log where f_order=:f_id and f_state=1")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(db.nextRow()) {
    //     jtax = str_json(db.getString("f_out"));
    // }

    // db[":f_id"] = fHeader;

    // if(!db.exec("select * from o_pay_cl where f_id=:f_id")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(!db.nextRow()) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // db.rowToMap(clinfo);
    // db[":f_id"] = fHeader;

    // if(!db.exec("select * from o_pay_room where f_id=:f_id")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(!db.nextRow()) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // db.rowToMap(roominfo);
    // db[":f_id"] = options["f_car"].toInt();

    // if(!db.exec("select * from b_car where f_id=:f_id")) {
    //     fError = db.fLastError;
    //     return false;
    // }

    // if(db.nextRow()) {
    //     db.rowToMap(carinfo);
    // }

    // db[":f_trsale"] = fHeader;
    // db.exec("select h.f_value, i.f_amount from b_gift_card_history i "
    //         "left join b_history h on h.f_id=i.f_trsale "
    //         "where h.f_id=:f_trsale");

    // if(db.nextRow()) {
    //     db.rowToMap(giftcardinfo);
    // }

    // db[":f_trsale"] = fHeader;
    // db.exec("select c.f_code, concat(p.f_contact) as f_costumer, sum(h.f_amount) as f_totalamount  "
    //         "from b_gift_card_history h "
    //         "left join b_cards_discount c on c.f_id=h.f_card "
    //         "left join c_partners p on p.f_id=c.f_client "
    //         "where h.f_card in (select f_card from b_gift_card_history where f_trsale=:f_trsale)");
    // db.nextRow();
    // db.rowToMap(giftcardinfo);

    if(fBill) {
        p.image("./logo_bill.png", Qt::AlignHCenter);
    } else {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
    }

    p.br();
    p.setFontBold(true);
    p.ctext(__translator.tt("Receipt #") + QString("%1%2").arg(fHeaderInfo["f_prefix"].toString()).arg(
                fHeaderInfo["f_hallid"].toInt()));
    p.br();

    if(fHeaderInfo["f_otherid"].toInt() == PAYOTHER_PRIMECOST) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ctext(__translator.tt("Breakfast"));
        p.br();
        p.br(4);
        p.setFontSize(bs);
        p.setFontBold(false);
    }

    if(fHeaderInfo["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
            || fHeaderInfo["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ctext(__translator.tt("Preorder"));
        p.br();
        p.br(4);
        p.setFontSize(bs);
        p.setFontBold(false);
    }

    p.setFontBold(false);

    if(!jtax.isEmpty()) {
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(__translator.tt("Tax number"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(__translator.tt("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(__translator.tt("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(__translator.tt("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(__translator.tt("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        p.ltext(__translator.tt("Date"), 0);
        p.rtext(QDateTime::fromMSecsSinceEpoch(jtax["time"].toDouble()).toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.ltext(__translator.tt("(F)"), 0);
        p.br();
    }

    p.br(1);
    p.ltext(__translator.tt("Table"), 0);
    p.setFontSize(bs);
    p.setFontBold(true);
    p.rtext(QString("%1/%2").arg(fHeaderInfo["f_hallname"].toString(), fHeaderInfo["f_tablename"].toString()));
    p.setFontSize(bs);
    p.setFontBold(false);
    p.br();
    p.ltext(__translator.tt("Staff"), 0);
    p.rtext(fHeaderInfo["f_currentstaffname"].toString());
    p.br();

    if(options["f_guests"].toInt() > 0) {
        p.ltext(__translator.tt("Guests"), 0);
        p.rtext(options["f_guests"].toString());
        p.br();
    }

    p.br(2);
    p.line();
    p.br(2);
    p.br(4);
    p.line();
    p.br(1);
    double needtopay = fHeaderInfo["f_amounttotal"].toDouble();
    bool noservice = false, nodiscount = false;
    p.setFontBold(true);
    p.setFontSize(bs - 4);
    p.ltext(__translator.tt("Name"), 0);
    p.ltext(__translator.tt("Qty"), 330);
    p.ltext(__translator.tt("Price"), 410);
    p.rtext(__translator.tt("Amount"));
    p.br();
    p.br(2);
    p.line();
    p.br(1);

    for(int i = 0; i < fBodyInfo.count(); i++) {
        QMap<QString, QVariant>& m = fBodyInfo[i];

        if(m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }

        QString name;

        if(!m["f_adgcode"].toString().isEmpty()) {
            p.ltext(QString("%1: %2").arg(__translator.tt("Class"), m["f_adgcode"].toString()), 0);
            p.br();
        }

        name = __translator.td(m["f_dish"].toInt());

        if(m["f_canservice"].toInt() == 0) {
            noservice = true;
            name += "*";
        }

        if(m["f_candiscount"].toInt() == 0) {
            nodiscount = true;
            name += "**";
        }

        p.ltext(name, 0, 330);

        if(fHeaderInfo["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
                || fHeaderInfo["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
            m["f_qty2"] = m["f_qty1"];
            m["f_total"] = m["f_qty1"].toDouble() * m["f_price"].toDouble();
            double srvAmount = 0;

            if(m["f_canservice"].toInt() != 0) {
                srvAmount = (m["f_total"].toDouble() * fHeaderInfo["f_servicefactor"].toDouble());
            }

            needtopay += m["f_total"].toDouble() + srvAmount;
            fHeaderInfo["f_amountservice"] = fHeaderInfo["f_amountservice"].toDouble() + srvAmount;
            fHeaderInfo["f_amounttotal"] = needtopay;
        }

        QString totalStr = m["f_total"].toDouble() > 0.001 ? float_str(m["f_total"].toDouble(), 2) : __translator.tt("Present");

        if(str_float(totalStr) < 0.001) {
            p.rtext(totalStr);
        } else {
            if(m["f_hourlypayment"].toInt() > 0) {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.rtext(float_str(m["f_total"].toDouble(), 2));
            } else {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.ltext(float_str(m["f_price"].toDouble(), 2), 410, 100);
                p.rtext(totalStr);
            }
        }

        if(m["f_extra"].toInt() > 0) {
            p.br();
            p.ltext(m["f_comment"].toString(), 0);
        }

        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.br();
    p.ltext(__translator.tt("Total"), 0);
    double printTotal = fHeaderInfo["f_amounttotal"].toDouble()
                        + fHeaderInfo["f_amountdiscount"].toDouble()
                        - fHeaderInfo["f_amountservice"].toDouble();
    p.rtext(float_str(printTotal, 2));
    p.br();
    p.br(1);

    if(fHeaderInfo["f_amountservice"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Service")).arg(fHeaderInfo["f_servicefactor"].toDouble() * 100), 0);
        p.rtext(float_str(fHeaderInfo["f_amountservice"].toDouble(), 2));
        p.br();
        p.br(1);
    }

    if(fHeaderInfo["f_discountfactor"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Discount"), float_str(fHeaderInfo["f_discountfactor"].toDouble() * 100,
                                      2)), 0);
        p.rtext(float_str(fHeaderInfo["f_amountdiscount"].toDouble(), 2));
        p.br();
        p.br(1);
    }

    //CARDINFO
    if(giftcardinfo.count() > 0 && giftcardinfo["f_amount"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2% (%3)").arg(__translator.tt("Accumulate"),
                                           float_str(giftcardinfo["f_value"].toDouble(), 2),
                                           float_str(giftcardinfo["f_amount"].toDouble(), 2)), 0);
        p.rtext(float_str(giftcardinfo["f_totalamount"].toDouble(), 2));
        p.br();
        //        p.ltext(QString("%1 %2").arg(__translator.tt("Card"), giftcardinfo["f_code"].toString()), 0);
        //        p.br();
        p.ltext(giftcardinfo["f_costumer"].toString(), 0);
        p.br();
    }

    double prepaid = preorder["f_prepaidcash"].toDouble()
                     + preorder["f_prepaidcard"].toDouble()
                     + preorder["f_prepaidpayx"].toDouble();

    if(prepaid > 0) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Prepaid amount"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(prepaid * -1, 2));
        needtopay -= prepaid;
        p.br();
        p.br();
    }

    p.br();
    p.line();
    p.br(2);
    p.line();
    p.br();
    p.setFontSize(bs + 4);

    if(needtopay > 0.01) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Need to pay"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(needtopay, 2));
    } else {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ltext(__translator.tt("Refund"), 0);
        p.setFontSize(bs + 8);
        p.rtext(float_str(needtopay, 2));
    }

    p.br();
    p.br();
    p.line();
    p.setFontBold(true);
    p.setFontSize(bs - 4);

    if(noservice) {
        p.ltext(QString("* - %1").arg(__translator.tt("No service")), 0);
        p.br();
    }

    if(nodiscount) {
        p.ltext(QString("** - %1").arg(__translator.tt("No discount")), 0);
        p.br();
    }

    p.br();
    p.line();

    // if(fIdram[param_idram_id].length() > 0 && fBill) {
    //     p.br();
    //     p.br();
    //     p.br();
    //     p.br();
    //     p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
    //     p.br();
    //     int levelIndex = 1;
    //     int versionIndex = 0;
    //     bool bExtent = true;
    //     int maskIndex = -1;
    //     QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
    //                            .arg(fIdram[param_idram_name])
    //                            .arg(fIdram[param_idram_id]) //IDram ID
    //                            .arg(fHeaderInfo["f_amounttotal"].toDouble())
    //                            .arg(fHeader)
    //                            .arg(fIdram[param_idram_phone])
    //                            .arg(fIdram[param_idram_tips].toInt() == 1 ? "1" : "0")
    //                            .arg(fIdram[param_idram_tips].toInt() == 1 ? fIdram[param_idram_tips_wallet] : "");
    //     CQR_Encode qrEncode;
    //     bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
    //                               encodeString.toUtf8().data());

    //     if(!successfulEncoding) {
    //         //            fLog.append("Cannot encode qr image");
    //     }

    //     int qrImageSize = qrEncode.m_nSymbleSize;
    //     int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
    //     QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    //     encodeImage.fill(1);

    //     for(int i = 0; i < qrImageSize; i++) {
    //         for(int j = 0; j < qrImageSize; j++) {
    //             if(qrEncode.m_byModuleData[i][j]) {
    //                 encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
    //             }
    //         }
    //     }

    //     QPixmap pix = QPixmap::fromImage(encodeImage);
    //     pix = pix.scaled(300, 300);
    //     p.image(pix, Qt::AlignHCenter);
    //     p.br();
    //     /* End QRCode */
    // } else {
    // }

    if(!fBill) {
        if(fHeaderInfo["f_amountcash"].toDouble() > 0.001) {
            double vp = fHeaderInfo["f_amountcash"].toDouble();
            p.ltext(__translator.tt("Payment, cash"), 0);
            p.rtext(float_str(vp, 2));
            p.br();
        }

        if(fHeaderInfo["f_amountcard"].toDouble() > 0.001) {
            p.ltext(__translator.tt("Payment, card"), 0);
            p.rtext(float_str(fHeaderInfo["f_amountcard"].toDouble(), 2));
            p.br();
        }

        if(fHeaderInfo["f_amountbank"].toDouble() > 0.001) {
            p.ltext(__translator.tt("Bank transfer"), 0);
            p.rtext(float_str(fHeaderInfo["f_amountbank"].toDouble(), 2));
            p.br();
        }

        if(fHeaderInfo["f_amountidram"].toDouble() > 0.001) {
            p.ltext(__translator.tt("Idram"), 0);
            p.rtext(float_str(fHeaderInfo["f_amountidram"].toDouble(), 2));
            p.br();
        }

        if(fHeaderInfo["f_amountpayx"].toDouble() > 0.001) {
            p.ltext(__translator.tt("PayX"), 0);
            p.rtext(float_str(fHeaderInfo["f_amountpayx"].toDouble(), 2));
            p.br();
        }

        if(fHeaderInfo["f_cash"].toDouble() > 0.01) {
            p.ltext(__translator.tt("Received cash"), 0);
            p.rtext(float_str(fHeaderInfo["f_cash"].toDouble(), 2));
            p.br();
            p.ltext(__translator.tt("Change"), 0);
            p.rtext(float_str(fHeaderInfo["f_change"].toDouble(), 2));
            p.br();
        }

        p.br();
    }

    p.setFontSize(bs + 2);

    if(!roominfo["f_res"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt("Transfer to room"));
        p.br();
        p.ctext(roominfo["f_room"].toString() + ", " + roominfo["f_guest"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt("Signature"));
        p.br(p.fLineHeight * 2);
    }

    if(!clinfo["f_code"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt("City ledger"));
        p.br();
        p.ctext(clinfo["f_code"].toString() + ", " + clinfo["f_name"].toString());
        p.br(p.fLineHeight * 3);
    }

    if(fHeaderInfo["f_otherid"].toInt() == PAYOTHER_DEBT) {
        p.br();
        p.ctext(__translator.tt("Debt") + " " + clinfo["f_name"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt("Signature"));
        p.br(p.fLineHeight * 2);
    }

    if(fHeaderInfo["f_otherid"].toInt() == PAYOTHER_COMPLIMENTARY) {
        p.br();
        p.ctext(__translator.tt("Complimentary") + " " + fHeaderInfo["f_comment"].toString());
        p.br(p.fLineHeight * 3);
        p.line(3);
        p.ctext(__translator.tt("Signature"));
        p.br(p.fLineHeight * 2);
    }

    // if(!guestInfo.isEmpty()) {
    //     p.ctext(__translator.tt("Customer"));
    //     p.br();
    //     p.ctext(guestInfo["f_phone"].toString());
    //     p.br();
    //     p.ctext(guestInfo["f_contact"].toString());
    //     p.br();
    //     p.ctext(guestInfo["f_address"].toString());
    //     p.br();
    // }
    p.setFontSize(bs);
    p.setFontBold(true);
    //p.ltext(__translator.tt("Thank you for visit!"), 0);
    p.br();

    if(fBill) {
        p.ltext(QString("%1: %2").arg(__translator.tt("Sample")).arg(abs(fHeaderInfo["f_precheck"].toInt()) + 1), 0);
    } else {
        p.ltext(QString("%1: %2").arg(__translator.tt("Sample")).arg(abs(fHeaderInfo["f_print"].toInt()) + 1), 0);
    }

    p.br();

    if(!fHeaderInfo["f_comment"].toString().isEmpty()) {
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

    p.ltext(__translator.tt("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPageSize ps(QPageSize::Custom);
    p.print(fPrinter, ps);
#else
    p.print(fPrinter, QPageSize::Custom);
#endif
    return true;
}
