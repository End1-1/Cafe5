#include "sqlquery.h"
#include "logwriter.h"
#include "commandline.h"
#include "printtaxn.h"
#include "c5networkdb.h"
#include "c5tr.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSettings>

bool fiscal(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    QElapsedTimer et;
    et.start();
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    QString configFile = path + "/fiscal.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        err = "Server not configured";
        return false;
    }
    QSettings s(configFile, QSettings::IniFormat);
    QString sql = QString("select sf_bill_info('{\"id\":\"%1\"}')").arg(jreq["order"].toString());
    jret["order"] = jreq["order"].toString();
    C5NetworkDB db(sql, s.value("host/host").toString());
    if (!db.query()) {
        return false;
    }
    QJsonArray jaaj;
    QJsonObject jo = db.fJsonOut["data"].toObject();
    jaaj = jo["data"].toArray();
    jaaj = jaaj[0].toArray();
    jo = QJsonDocument::fromJson(jaaj[0].toString().toUtf8()).object();
    jo = jo["data"].toObject();
    QJsonObject jConfig = jo["config"].toObject();
    QJsonObject jHeader = jo["header"].toObject();
    QJsonArray jDishes = jo["dishes"].toArray();
    QJsonObject jfiscal = jo["fiscal"].toObject();
    int fiscal_result = 0;
    if (jfiscal["rseq"].toInt() == 0) {
        PrintTaxN pt(jConfig["fiscal_ip"].toString(),
                     jConfig["fiscal_port"].toInt(),
                     jConfig["fiscal_password"].toString(),
                     jConfig["fiscal_extpos"].toString(),
                     jConfig["fiscal_opcode"].toString(),
                     jConfig["fiscal_oppin"].toString());
        double amountcard = jHeader["f_amountcard"].toDouble() + jHeader["f_amountidram"].toDouble();
        for (int i = 0; i < jDishes.size(); i++) {
            QJsonObject jdish = jDishes.at(i).toObject();
            pt.addGoods(jdish["f_taxdept"].toInt(),
                        jdish["f_adgt"].toString(),
                        jdish["f_id"].toString(),
                        jdish["f_name_am"].toString(),
                        jdish["f_price"].toDouble(),
                        jdish["f_qty1"].toDouble(),
                        jdish["f_discount"].toDouble());
        }
        QString inJson, outJson, err;
        fiscal_result = pt.makeJsonAndPrint(amountcard, 0, inJson, outJson, err);
#ifdef QT_DEBUG
        if (fiscal_result != 0) {
            if (jreq["debug_fiscal_ok"].toBool()) {
                fiscal_result = 0;
                outJson = "{\"address\": \"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Հյուսիսային պողոտա 1/1 \", "
                          "\"change\": 0, \"crn\": \"53241359\", \"fiscal\": \"81326142\", "
                          "\"lottery\": \"\", \"prize\": 0, \"rseq\": 7174, \"sn\": \"00022164315\", "
                          "\"taxpayer\": \"«ՍՄԱՐՏ ՊԱՐԿԻՆԳ»\", \"time\": 1733052744891, \"tin\": \"02853428\", \"total\": 1}";
            }
        }
#endif
        QJsonObject jtax;
        QJsonParseError jsonErr;
        QJsonObject fJsonOut;
        jtax["f_order"] = jHeader["f_id"].toString();
        jtax["f_elapsed"] = et.elapsed();
        jtax["f_in"] = QJsonDocument::fromJson(inJson.toUtf8(), &jsonErr).object();
        jtax["f_out"] = QJsonDocument::fromJson(outJson.toUtf8()).object();;
        jtax["f_err"] = err;
        jtax["f_result"] = fiscal_result;
        jtax["f_state"] = fiscal_result == pt_err_ok ? 1 : 0;
        if (jsonErr.error != QJsonParseError::NoError) {
            fJsonOut["data"] = jsonErr.errorString();
        }
        if (!db.query(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(
                          QJsonDocument::Compact))))) {
            fJsonOut["data"] = db.fLastError;
        }
        if (fiscal_result != pt_err_ok) {
            fJsonOut["data"] = err;
        }
        jret["fiscal_result"] = fiscal_result;
        jret["fiscal_data"]  = jtax["f_out"];
    }
    if (fiscal_result != 0) {
        jret["errorCode"] = fiscal_result;
        if (fiscal_result > 0) {
            jret["errorMessage"] = err;
        }
        err = QJsonDocument(jret).toJson(QJsonDocument::Compact);
        return false;
    }
    return true;
}

bool printbill(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    QString configFile = path + "/fiscal.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        err = "Server not configured";
        return false;
    }
    QSettings s(configFile, QSettings::IniFormat);
    QString sql = QString("select sf_bill_info('{\"id\":\"%1\"}')").arg(jreq["order"].toString());
    C5NetworkDB db(sql, s.value("host/host").toString());
    if (!db.query()) {
        return false;
    }
    QJsonArray jaaj;
    QJsonObject jo = db.fJsonOut["data"].toObject();
    jaaj = jo["data"].toArray();
    jaaj = jaaj[0].toArray();
    jo = QJsonDocument::fromJson(jaaj[0].toString().toUtf8()).object();
    jo = jo["data"].toObject();
    QJsonObject jConfig = jo["config"].toObject();
    QJsonObject jHeader = jo["header"].toObject();
    QJsonObject jFiscal = jo["fiscal"].toObject();
    QJsonArray jDishes = jo["dishes"].toArray();
    QJsonArray jTranslator = jo["translator"].toArray();
    QJsonObject jCar = jo["car"].toObject();
    int bs = jConfig["recipe_font_size"].toInt();
    C5Tr __translator;
    __translator.setLanguage("am");
    for (int i = 0; i < jTranslator.size(); i++) {
        QJsonObject jt = jTranslator.at(i).toObject();
        __translator.add(jt["f_en"].toString(), jt["f_am"].toString(), jt["f_ru"].toString());
    }
    QFont font(jConfig["recipe_font_family"].toString());
    font.setPointSize(bs);
    C5Printing p;
    p.setSceneParams(jConfig["recipe_paper_width"].toInt(), 2600, QPrinter::Portrait);
    p.setFont(font);
    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.setFontBold(true);
    p.ctext(__translator.tt("Receipt #") + QString("%1%2").arg(jHeader["f_prefix"].toString()).arg(
                jHeader["f_hallid"].toInt()));
    p.br();
    if (jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
            || jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        p.setFontSize(bs + 4);
        p.setFontBold(true);
        p.ctext(__translator.tt("Preorder"));
        p.br();
        p.br(4);
        p.setFontSize(bs);
        p.setFontBold(false);
    }
    p.setFontBold(false);
    if (jFiscal["rseq"].toInt() > 0) {
        p.ltext(jFiscal["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jFiscal["address"].toString(), 0);
        p.br();
        //        p.ltext(__translator.tt("Department"), 0);
        //        p.rtext(taxinfo["f_dept"].toString());
        //        p.br();
        p.ltext(__translator.tt("Tax number"), 0);
        p.rtext(jFiscal["tin"].toString());
        p.br();
        p.ltext(__translator.tt("Device number"), 0);
        p.rtext(jFiscal["crn"].toString());
        p.br();
        p.ltext(__translator.tt("Serial"), 0);
        p.rtext(jFiscal["sn"].toString());
        p.br();
        p.ltext(__translator.tt("Fiscal"), 0);
        p.rtext(jFiscal["fiscal"].toString());
        p.br();
        p.ltext(__translator.tt("Receipt number"), 0);
        p.rtext(QString::number(jFiscal["rseq"].toInt()));
        p.br();
        //        p.ltext(__translator.tt("Date"), 0);
        //        p.rtext(taxinfo["f_taxtime"].toString());
        //        p.br();
        p.ltext(__translator.tt("(F)"), 0);
        p.br();
    }
    p.br(1);
    p.ltext(__translator.tt("Table"), 0);
    p.setFontSize(bs);
    p.setFontBold(true);
    p.rtext(QString("%1/%2").arg(jHeader["f_hallname"].toString(), jHeader["f_tablename"].toString()));
    p.setFontSize(bs);
    p.setFontBold(false);
    p.br();
    p.ltext(__translator.tt("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(__translator.tt("Staff"), 0);
    p.rtext(jHeader["f_currentstaffname"].toString());
    p.br();
    if (!jCar["carnumber"].toString().isEmpty()) {
        p.ltext(__translator.tt("Car number"), 0);
        p.rtext(jCar["carnumber"].toString());
        p.br();
    }
    p.br(2);
    p.line();
    p.br(2);
    p.br(4);
    p.line();
    p.br(1);
    double needtopay = jHeader["f_amounttotal"].toDouble();
    if (jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
            || jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        needtopay = 0;
    }
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
    for (int i = 0; i < jDishes.count(); i++) {
        QJsonObject m = jDishes[i].toObject();
        if (m["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        QString name;
        if (!m["f_adgcode"].toString().isEmpty()) {
            p.ltext(QString("%1: %2").arg(__translator.tt("Class"), m["f_adgcode"].toString()), 0);
            p.br();
        }
        name = m["f_name_am"].toString();
        if (m["f_canservice"].toInt() == 0) {
            noservice = true;
            name += "*";
        }
        if (m["f_candiscount"].toInt() == 0) {
            nodiscount = true;
            name += "**";
        }
        p.ltext(name, 0, 330);
        if (jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
                || jHeader["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
            m["f_qty2"] = m["f_qty1"];
            m["f_total"] = m["f_qty1"].toDouble() * m["f_price"].toDouble();
            double srvAmount = 0;
            if (m["f_canservice"].toInt() != 0) {
                srvAmount = (m["f_total"].toDouble() * jHeader["f_servicefactor"].toDouble());
            }
            needtopay += m["f_total"].toDouble() + srvAmount;
            jHeader["f_amountservice"] = jHeader["f_amountservice"].toDouble() + srvAmount;
            jHeader["f_amounttotal"] = needtopay;
        }
        QString totalStr = m["f_total"].toDouble() > 0.001 ? float_str(m["f_total"].toDouble(), 2) : __translator.tt("Present");
        if (str_float(totalStr) < 0.001) {
            p.rtext(totalStr);
        } else {
            if (m["f_qty2"].toDouble() < 0.01) {
                m["f_qty2"] = m["f_qty1"];
            }
            if (m["f_hourlypayment"].toInt() > 0) {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.rtext(float_str(m["f_total"].toDouble(), 2));
            } else {
                p.ltext(float_str(m["f_qty2"].toDouble(), 2), 330, 100);
                p.ltext(float_str(m["f_price"].toDouble(), 2), 410, 120);
                p.rtext(totalStr);
            }
        }
        if (m["f_extra"].toInt() > 0) {
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
    double printTotal = jHeader["f_amounttotal"].toDouble()
                        + jHeader["f_amountdiscount"].toDouble()
                        - jHeader["f_amountservice"].toDouble();
    p.rtext(float_str(printTotal, 2));
    p.br();
    p.br(1);
    if (jHeader["f_amountservice"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Service")).arg(jHeader["f_servicefactor"].toDouble() * 100), 0);
        p.rtext(float_str(jHeader["f_amountservice"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    if (jHeader["f_discountfactor"].toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt("Discount"), float_str(jHeader["f_discountfactor"].toDouble() * 100, 2)),
                0);
        p.rtext(float_str(jHeader["f_amountdiscount"].toDouble(), 2));
        p.br();
        p.br(1);
    }
    //        double prepaid = preorder["f_prepaidcash"].toDouble()
    //                         + preorder["f_prepaidcard"].toDouble()
    //                         + preorder["f_prepaidpayx"].toDouble();
    double prepaid = 0;
    if (prepaid > 0) {
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
    if (needtopay > 0.01) {
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
    if (noservice) {
        p.ltext(QString("* - %1").arg(__translator.tt("No service")), 0);
        p.br();
    }
    if (nodiscount) {
        p.ltext(QString("** - %1").arg(__translator.tt("No discount")), 0);
        p.br();
    }
    p.br();
    p.line();
    //    if (fIdram[param_idram_id].length() > 0 && fBill){
    //        C5LogSystem::writeEvent("Idram QR");
    //        p.br();
    //        p.br();
    //        p.br();
    //        p.br();
    //        p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
    //        p.br();
    //        int levelIndex = 1;
    //        int versionIndex = 0;
    //        bool bExtent = true;
    //        int maskIndex = -1;
    //        QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
    //                                   .arg(fIdram[param_idram_name])
    //                                   .arg(fIdram[param_idram_id]) //IDram ID
    //                                   .arg(fHeaderInfo["f_amounttotal"].toDouble())
    //                                   .arg(fHeader)
    //                                   .arg(fIdram[param_idram_phone])
    //                                   .arg(fIdram[param_idram_tips].toInt() == 1 ? "1" : "0")
    //                                   .arg(fIdram[param_idram_tips].toInt() == 1 ? fIdram[param_idram_tips_wallet] : "");
    //        CQR_Encode qrEncode;
    //        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
    //        if (!successfulEncoding) {
    //            //            fLog.append("Cannot encode qr image");
    //        }
    //        int qrImageSize = qrEncode.m_nSymbleSize;
    //        int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    //        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    //        encodeImage.fill(1);
    //        for ( int i = 0; i < qrImageSize; i++ ) {
    //            for ( int j = 0; j < qrImageSize; j++ ) {
    //                if ( qrEncode.m_byModuleData[i][j] ) {
    //                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
    //                }
    //            }
    //        }
    //        QPixmap pix = QPixmap::fromImage(encodeImage);
    //        pix = pix.scaled(300, 300);
    //        p.image(pix, Qt::AlignHCenter);
    //        p.br();
    //        /* End QRCode */
    //    } else {
    //    }
    if (jHeader["f_amountcash"].toDouble() > 0.001) {
        double vp = jHeader["f_amountcash"].toDouble();
        p.ltext(__translator.tt("Payment, cash"), 0);
        p.rtext(float_str(vp, 2));
        p.br();
    }
    if (jHeader["f_amountcard"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Payment, card"), 0);
        p.rtext(float_str(jHeader["f_amountcard"].toDouble(), 2));
        p.br();
    }
    if (jHeader["f_amountbank"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Bank transfer"), 0);
        p.rtext(float_str(jHeader["f_amountbank"].toDouble(), 2));
        p.br();
    }
    if (jHeader["f_amountidram"].toDouble() > 0.001) {
        p.ltext(__translator.tt("Idram"), 0);
        p.rtext(float_str(jHeader["f_amountidram"].toDouble(), 2));
        p.br();
    }
    if (jHeader["f_amountpayx"].toDouble() > 0.001) {
        p.ltext(__translator.tt("PayX"), 0);
        p.rtext(float_str(jHeader["f_amountpayx"].toDouble(), 2));
        p.br();
    }
    //    if (fHeaderInfo["f_cash"].toDouble() > 0.01) {
    //        p.ltext(__translator.tt("Received cash"), 0);
    //        p.rtext(float_str(fHeaderInfo["f_cash"].toDouble(), 2));
    //        p.br();
    //        p.ltext(__translator.tt("Change"), 0);
    //        p.rtext(float_str(fHeaderInfo["f_change"].toDouble(), 2));
    //        p.br();
    //    }
    p.br();
    p.setFontSize(bs + 2);
    p.setFontSize(bs);
    p.setFontBold(true);
    //p.ltext(__translator.tt("Thank you for visit!"), 0);
    p.br();
    p.ltext(QString("%1: %2").arg(__translator.tt("Sample")).arg(abs(jHeader["f_precheck"].toInt()) + 1), 0);
    p.br();
    if (!jHeader["f_comment"].toString().isEmpty()) {
        p.br();
        p.br();
        p.br();
        p.br();
        p.br();
        p.line();
        p.br();
        p.ltext(jHeader["f_comment"].toString(), 0);
        p.br();
    }
    p.print(jConfig["recipe_printer"].toString(), QPrinter::Custom);
    sql = QString("update o_header set f_print=abs(coalesce(f_print, 0))+1 where f_id='%1'").arg(
              jHeader["f_id"].toString());
    db.query(sql);
    if (jreq["error"].toInt() > 0) {
        err = "Artificial Error";
        return false;
    }
    return true;
}
