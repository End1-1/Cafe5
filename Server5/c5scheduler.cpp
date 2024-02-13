#include "c5scheduler.h"
#include "server5settings.h"
#include "c5database.h"
#include "c5networkdb.h"
#include "c5utils.h"
#include "c5printing.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonParseError>

c5scheduler::c5scheduler(QObject *parent) : QObject(parent)
{
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
#ifdef QT_DEBUG
    fTimer.setInterval(10000 * __s.value("updateinterval").toInt());
#else
    fTimer.setInterval(1000 * 60 * __s.value("updateinterval").toInt());
#endif
    fTimer.start();
    fRun = false;
#ifdef QT_DEBUG
    timeout();
#endif
}

void c5scheduler::timeout()
{
    if (fRun) {
        return;
    }
    fRun = true;
    if (__s.value("url").toString().isEmpty()) {
        fRun = false;
        return;
    }
    runServicePrint();
    runMessages();
    fRun = false;
}

void c5scheduler::runServicePrint()
{
    QString originalPrinter = __s.value("printername").toString();
    QString sql = QString("select sf_print_service('{\"printer\":\"%1\"}')").arg(originalPrinter);
    C5NetworkDB db(sql, __s.value("url").toString(), this);
    if (!db.query()) {
        return;
    }
    QJsonObject jo;
    QString strData = db.fJsonOut["data"].toObject()["data"].toArray().at(0).toArray().at(0).toString();
    jo = QJsonDocument::fromJson(strData.toUtf8()).object();
    jo = jo["data"].toObject();
    if (jo["orders"].toArray().isEmpty()) {
        return;
    }
    QJsonObject jconfig = jo["config"].toObject();
    QString printField = __s.value("printerside").toString();

    int fontSize = jconfig["font_size"].toInt();
    QFont font(jconfig["font_family"].toString(), fontSize);
    font.setPointSize(20);
    QJsonArray jcomplete;
    for (int i = 0; i < jo["orders"].toArray().size(); i++) {
        C5Printing p;
        p.setSceneParams(600, 2800, QPrinter::Portrait);
        p.setFont(font);

        QJsonObject jfirst = jo["orders"].toArray().at(i).toObject();
        if (jfirst["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY || jfirst["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
            p.setFontSize(32);
            p.setFontBold(true);
            p.ctext("Նախնական պատվեր");
            p.br();
            // p.ltext(fPreorderData["f_datefor"].toDate().toString(FORMAT_DATE_TO_STR), 0);
            // p.rtext(fPreorderData["f_timefor"].toTime().toString(FORMAT_TIME_TO_STR));
            p.br();
            //p.ltext(tr("Guests"), 0);
            // p.rtext(fPreorderData["f_guests"].toString());
            p.br();
            p.br();
            p.line();
            p.br();
        }

        // if (reprint) {
        //     p.setFontSize(34);
        //     p.setFontBold(true);
        //     p.ctext(tr("REPRINT"));
        //     p.br();
        //     p.br();
        // }

        p.setFontBold(false);
        p.setFontSize(fontSize);
        p.ctext("Նոր պատվեր");
        p.br();
        p.ltext("Սեղան", 0);
        p.setFontSize(28);
        p.rtext(jfirst["f_tablename"].toString());
        p.br();
        p.setFontSize(20);
        p.ltext(tr("Պատվեր"), 0);
        p.rtext(jfirst["f_hallid"].toString());
        p.br();
        p.ltext(tr("Ամսաթիվ"), 0);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Ժամ"), 0);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Սպասարկող"), 0);
        p.rtext(jfirst["f_staffname"].toString());
        p.br(p.fLineHeight + 2);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(2);

        p.setFontSize(30);
        QSet<QString> storages;
        bool toprint = false;
        for (int i = 0; i < jfirst["items"].toArray().size(); i++) {
            QJsonObject jdish = jfirst["items"].toArray().at(i).toObject();
            if (jdish[printField] != originalPrinter) {
                continue;
            }
            toprint = true;
            jcomplete.append(jdish["f_id"].toString());

            // storages << dbstore->name(o["f_store"].toInt());
            // if (__c5config.getValue(param_print_dish_timeorder).toInt() == 1) {
            //     p.ltext(QString("[%1] %2").arg(o["f_timeorder"].toString(), dbdish->name(o["f_dish"].toInt())), 0);
            // } else {
            //     p.ltext(QString("%1").arg(dbdish->name(o["f_dish"].toInt())), 0);
            // }
            p.ltext(QString("%1").arg(jdish["f_dishname"].toString()), 0);
            p.setFontBold(true);
            p.rtext(QString("%1").arg(float_str(jdish["f_qty1"].toDouble(), 2)));
            p.setFontBold(false);

            if (jdish["f_comment2"].toString().length() > 0) {
                p.br();
                p.setFontSize(25);
                p.setFontBold(true);
                p.ltext(jdish["f_comment2"].toString(), 0);
                p.br();
                p.setFontSize(30);
                p.setFontBold(false);
            }

            if (jdish["f_comment"].toString().length() > 0) {
                p.br();
                p.setFontSize(25);
                p.ltext(jdish["f_comment"].toString(), 0);
                p.br();
                p.setFontSize(30);
            }

            p.br();
            p.line(0, p.fTop, p.fNormalWidth, p.fTop);
            p.br(1);
        }
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.setFontBold(true);
        p.setFontSize(fontSize + 6);
        p.ltext("Վճարումը՝", 0);
        p.br();
        if (jfirst["f_amountcash"].toDouble() > 0) {
            p.ltext("Կանխիկ", 0);
            p.br();
        }
        if (jfirst["f_amountcard"].toDouble() > 0) {
            p.ltext("Քարտ", 0);
            p.br();
        }
        if (jfirst["f_amountidram"].toDouble() > 0) {
            p.ltext("Իդրամ", 0);
            p.br();
        }
        p.br();
        p.br();
        p.setFontSize(20);
        p.ltext(tr("Printer: ") + originalPrinter, 0);
        p.setFontBold(true);
        p.rtext(printField == "f_print1" ? " [1]" : "[2]");
        p.br();
        //p.ltext(tr("Storage: ") + storages.toList().join(","), 0);

        // if (fPrinterAliases.contains(printer)) {
        //     printer = fPrinterAliases[printer];
        // }
        if (!toprint) {
            continue;
        }
        QString final = "OK";
        if (!p.print(originalPrinter, QPrinter::Custom)) {
            final = "FAIL";
        }
    }

    if (!jcomplete.isEmpty()) {
        jo = QJsonObject();
        jo["ids"] = jcomplete;
        jo["side"] = printField == "f_print1" ? 1 : 2;
        sql = QString("select sf_print_complete('%1')")
                  .arg(QString(QJsonDocument(jo).toJson(QJsonDocument::Compact)));
        db.query(sql);
    }

}

void c5scheduler::runMessages()
{
    QString originalPrinter = __s.value("printername").toString();
    QString sql = QString("select sf_get_messages('{\"f_destination\":\"%1\"}')").arg(originalPrinter);
    C5NetworkDB db(sql, __s.value("url").toString(), this);
    if (!db.query()) {
        return;
    }
    QString strData = db.fJsonOut["data"].toObject()["data"].toArray().at(0).toArray().at(0).toString();
    QJsonObject jo = QJsonDocument::fromJson(strData.toUtf8()).object();
    QJsonArray jma = jo["data"].toArray();
    QJsonArray jc;
    for (int i = 0; i < jma.size(); i++) {
        jc.append(jma.at(i)["f_id"].toInt());
        QJsonObject jm = jma.at(i).toObject()["f_data"].toObject();
        C5Printing p;
        p.setSceneParams(600, 2800, QPrinter::Portrait);
        p.setFont(QFont("Arial LatArm Unicode", 29));


            p.setFontSize(32);
            p.setFontBold(true);
            p.ctext("Անհրաժեշտ է օգնություն");
            p.br();
            p.line();
            p.br();


        // if (reprint) {
        //     p.setFontSize(34);
        //     p.setFontBold(true);
        //     p.ctext(tr("REPRINT"));
        //     p.br();
        //     p.br();
        // }

        p.setFontBold(false);
        p.setFontSize(20);
        p.ltext("Սեղան", 0);
        p.setFontSize(28);
        p.rtext(jm["f_tablename"].toString());
        p.br();
        p.setFontSize(20);
        p.ltext(tr("Date"), 0);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Time"), 0);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(2);
        p.setFontSize(30);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.setFontSize(20);
        p.ltext("Հաղորդագրություն", 0);
        p.br();
        p.line();
        p.br();
        p.setFontSize(28);
        p.ltext(jm["f_message"].toString(), 0);
        p.br();
        QString final = "OK";
        if (!p.print(originalPrinter, QPrinter::Custom)) {
            final = "FAIL";
        }
    }
    if (!jc.isEmpty()) {
        sql = QString("select sf_set_messages_complete('{\"ids\":\"%1\"}')").arg(
            QString(QJsonDocument(jc).toJson(QJsonDocument::Compact)));
        if (!db.query(sql)) {
            return;
        }
    }
}


