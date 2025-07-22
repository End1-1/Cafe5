#include "c5printservicethread.h"
#include "c5printing.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5database.h"
#include "c5tabledata.h"
#include <QApplication>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

C5PrintServiceThread::C5PrintServiceThread(const QString &header, QObject *parent) :
    QObject(parent)
{
    fHeader = header;
    fBooking = false;
    fUseAliases = false;
}

C5PrintServiceThread::~C5PrintServiceThread()
{
}

bool C5PrintServiceThread::run()
{
    QSet<QString> fPrint1, fPrint2;
    C5Database db;
    if (fUseAliases) {
        db.exec("select f_alias, f_printer from d_print_aliases");
        while (db.nextRow()) {
            fPrinterAliases[db.getString("f_alias")] = db.getString("f_printer");
        }
    }
    db[":f_id"] = fHeader;
    db.exec("select * from o_header where f_id=:f_id");
    if (db.nextRow()) {
        db.rowToMap(fHeaderData);
    }
    db[":f_id"] = fHeader;
    db.exec("select * from o_preorder where f_id=:f_id");
    if (db.nextRow()) {
        db.rowToMap(fPreorderData);
    }
    db[":f_header"] = fHeader;
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_SET;
    db.exec("select * from o_body b "
            "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) "
            "and (length(f_print1)>0 or length(f_print2)>0) and b.f_qty2=0 "
            "order by b.f_appendtime ");
    while (db.nextRow()) {
        QMap<QString, QVariant> m;
        db.rowToMap(m);
        fBodyData.append(m);
    }
    for (int i = 0; i < fBodyData.count(); i++) {
        if (fBodyData.at(i)["f_print1"].toString().length() > 0) {
            fPrint1 << fBodyData.at(i)["f_print1"].toString();
        }
        if (fBodyData.at(i)["f_print1"].toString() != fBodyData.at(i)["f_print2"].toString()) {
            if (fBodyData.at(i)["f_print2"].toString().length() > 0) {
                fPrint2 << fBodyData.at(i)["f_print2"].toString();
            }
        }
    }
    foreach (QString s, fPrint1) {
        print(s, "f_print1", false);
    }
    foreach (QString s, fPrint2) {
        print(s, "f_print2", false);
    }
    if (fHeaderData["f_state"].toInt() == ORDER_STATE_OPEN) {
        QStringList sqls;
        db[":f_header"] = fHeader;
        sqls.append(db.execDry("update o_body set f_qty2=f_qty1, f_reprint=abs(f_reprint) where f_header=:f_header"));
        for (int i = 0; i < fBodyData.count(); i++) {
            const QMap<QString, QVariant> &o = fBodyData.at(i);
            db[":f_id"] = o["f_id"];
            sqls.append(db.execDry("update o_body set f_printtime=current_timestamp() where f_id=:f_id"));
        }
        db.execSqlList(sqls);
    }
    if (!fReprintList.isEmpty()) {
        fBodyData.clear();
        db.exec(QString("select * from o_body where f_id in (%1) ").arg(fReprintList));
        while (db.nextRow()) {
            QMap<QString, QVariant> m;
            db.rowToMap(m);
            fBodyData.append(m);
        }
        fPrint1.clear();
        fPrint2.clear();
        for (int i = 0; i < fBodyData.count(); i++) {
            if (fBodyData.at(i)["f_print1"].toString().length() > 0) {
                fPrint1 << fBodyData.at(i)["f_print1"].toString();
            }
            if (fBodyData.at(i)["f_print1"].toString() != fBodyData.at(i)["f_print2"].toString()) {
                if (fBodyData.at(i)["f_print2"].toString().length() > 0) {
                    fPrint2 << fBodyData.at(i)["f_print2"].toString();
                }
            }
        }
        foreach (QString s, fPrint1) {
            print(s, "f_print1", true);
        }
        foreach (QString s, fPrint2) {
            print(s, "f_print2", true);
        }
    }
    return true;
}

void C5PrintServiceThread::print(QString printer, const QString &side, bool reprint)
{
    C5Database().logEvent(QString::number(__c5config.receiptParepWidth()));
    QString originalPrinter = printer;
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(__c5config.receiptParepWidth(), 2800, QPageLayout::Portrait);
    p.setFont(font);
    if (fHeaderData["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
            || fHeaderData["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER || fBooking) {
        p.setFontSize(32);
        p.setFontBold(true);
        p.ctext(tr("PREORDER"));
        p.br();
        p.ltext(fPreorderData["f_datefor"].toDate().toString(FORMAT_DATE_TO_STR), 0);
        p.rtext(fPreorderData["f_timefor"].toTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Guests"), 0);
        p.rtext(fPreorderData["f_guests"].toString());
        p.br();
        p.br();
        p.line();
        p.br();
    }
    if (reprint) {
        p.setFontSize(34);
        p.setFontBold(true);
        p.ctext(tr("REPRINT"));
        p.br();
        p.br();
    }
    p.setFontBold(false);
    p.setFontSize(20);
    p.ctext(tr("New order"));
    p.br();
    p.ltext(tr("Table"), 0);
    p.setFontSize(28);
    p.rtext(C5TableData::instance()->string("h_tables", "f_name", fHeaderData["f_table"].toInt()));
    p.br();
    p.setFontSize(20);
    p.ltext(tr("Order no"), 0);
    p.rtext(QString("%1%2").arg(fHeaderData["f_prefix"].toString()).arg(fHeaderData["f_hallid"].toInt()));
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(C5TableData::instance()->string("s_user", "f_last", fHeaderData["f_currentstaff"].toInt()) +  " "
            + C5TableData::instance()->string("s_user", "f_first", fHeaderData["f_currentstaff"].toInt()));
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(2);
    p.setFontSize(30);
    QSet<QString> storages;
    for (int i = 0; i < fBodyData.count(); i++) {
        const QMap<QString, QVariant> &o = fBodyData.at(i);
        if (o[side].toString() != printer) {
            continue;
        }
        storages << C5TableData::instance()->string("c_storages", "f_name", o["f_store"].toInt());
        if (__c5config.getValue(param_print_dish_timeorder).toInt() == 1) {
            p.ltext(QString("[%1] %2").arg(o["f_timeorder"].toString(), C5TableData::instance()->string("d_dish", "f_name",
                                           o["f_dish"].toInt())), 0);
        } else {
            p.ltext(QString("%1").arg(C5TableData::instance()->string("d_dish", "f_name", o["f_dish"].toInt())), 0);
        }
        p.setFontBold(true);
        p.rtext(QString("%1").arg(float_str(o["f_qty1"].toDouble(), 2)));
        p.setFontBold(false);
        if (C5TableData::instance()->variant("d_dish", "f_extra", o["f_dish"].toInt()).toInt()) {
            p.br();
            p.setFontSize(25);
            p.ltext(QString("%1: %2")
                    .arg(tr("Extra price"))
                    .arg(float_str(o["f_price"].toDouble(), 2)), 0);
        }
        if (o["f_comment2"].toString().length() > 0) {
            p.br();
            p.setFontSize(25);
            p.setFontBold(true);
            p.ltext(o["f_comment2"].toString(), 0);
            p.br();
            p.setFontSize(30);
            p.setFontBold(false);
        }
        if (o["f_comment"].toString().length() > 0) {
            p.br();
            p.setFontSize(25);
            p.ltext(o["f_comment"].toString(), 0);
            p.br();
            p.setFontSize(30);
        }
        p.br();
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
    }
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);
    p.setFontSize(20);
    p.ltext(tr("Printer: ") + printer, 0);
    p.setFontBold(true);
    p.rtext(side == "f_print1" ? " [1]" : "[2]");
    p.br();
    p.ltext(tr("Storage: ") + storages.values().join(","), 0);
    if (fPrinterAliases.contains(printer)) {
        printer = fPrinterAliases[printer];
    }
    QString final = "OK";
    if (!p.print(printer, QPageSize::Custom)) {
        final = "FAIL";
    }
    QDir dir;
    if (!dir.exists(dir.tempPath() + "/Waiter")) {
        dir.mkpath(dir.tempPath() + "/Waiter");
    }
    QString fileName = QString("%1/%2-%3%4-1-%5.txt")
                       .arg(dir.tempPath() + "/Waiter")
                       .arg(originalPrinter)
                       .arg(fHeaderData["f_prefix"].toString())
                       .arg(fHeaderData["f_hallid"].toInt())
                       .arg(final);
    int i = 2;
    while (QFile::exists(fileName)) {
        fileName = QString("%1/%2-%3%4-%5-%6.txt")
                   .arg(dir.tempPath() + "/Waiter")
                   .arg(printer)
                   .arg(fHeaderData["f_prefix"].toString())
                   .arg(fHeaderData["f_hallid"].toInt())
                   .arg(i++)
                   .arg(final);
    }
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly)) {
        QJsonDocument jdoc(p.jsonData());
        f.write(jdoc.toJson(QJsonDocument::Compact));
    }
    f.close();
}
