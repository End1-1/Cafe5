#include "c5printservicethread.h"
#include "c5printing.h"
#include "datadriver.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5database.h"
#include <QApplication>

C5PrintServiceThread::C5PrintServiceThread(const QString &header, QObject *parent) :
    QObject(parent)
{
    fHeader = header;
}

C5PrintServiceThread::~C5PrintServiceThread()
{

}

bool C5PrintServiceThread::run()
{
    QSet<QString> fPrint1, fPrint2;
    C5Database db(__c5config.dbParams());
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
    db[":f_state"] = DISH_STATE_OK;
    db.exec("select f_id, f_dish, f_timeorder, f_print1, f_print2, f_qty1 - f_qty2 as f_qty, f_comment "
            "from o_body b "
            "where b.f_header=:f_header and b.f_state=:f_state "
            "and (length(f_print1)>0 or length(f_print2)>0) and b.f_qty1-b.f_qty2>0 ");
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
            if (fBodyData.at(i)["f_print2"].toString() > 0) {
                fPrint2 << fBodyData.at(i)["f_print2"].toString();
            }
        }
    }

    foreach (QString s, fPrint1) {
        print(s, "f_print1");
    }

    foreach (QString s, fPrint2) {
        print(s, "f_print2");
    }

    if (fHeaderData["f_state"].toInt() == ORDER_STATE_OPEN) {
        db[":f_header"] = fHeader;
        db.exec("update o_body set f_qty2=f_qty1 where f_header=:f_header");
    }
    return true;
}

void C5PrintServiceThread::print(const QString &printer, const QString &side)
{
    QFont font(qApp->font());
    font.setPointSize(30);
    C5Printing p;
    p.setSceneParams(__c5config.receiptParepWidth(), 2800, QPrinter::Portrait);
    p.setFont(font);

    if (fHeaderData["f_state"].toInt() == ORDER_STATE_PREORDER_1 || fHeaderData["f_state"].toInt() == ORDER_STATE_PREORDER_2) {
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

    p.ctext(tr("New order"));
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(dbtable->name(fHeaderData["f_table"].toInt()));
    p.br();
    p.ltext(tr("Order no"), 0);
    p.rtext(QString("%1%2").arg(fHeaderData["f_prefix"].toString()).arg(fHeaderData["f_hallid"].toString()));
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(dbuser->fullName(fHeaderData["f_currentstaff"].toInt()));
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(2);

    QSet<QString> storages;
    for (int i = 0; i < fBodyData.count(); i++) {
        const QMap<QString, QVariant> &o = fBodyData.at(i);
        if (o[side].toString() != printer) {
            continue;
        }
        storages << dbstore->name(o["f_store"].toInt());
        if (__c5config.getValue(param_print_dish_timeorder).toInt() == 1) {
            p.ltext(QString("[%1] %2").arg(o["f_timeorder"].toString()).arg(dbdish->name(o["f_dish"].toInt())), 0);
        } else {
            p.ltext(QString("%1").arg(dbdish->name(o["f_dish"].toInt())), 0);
        }
        p.br();
        if (o["f_comment"].toString().length() > 0) {
            p.ltext(o["f_comment"].toString(), 0);
            p.br();
        }
        p.ctext(QString("%1: %2").arg(tr("Qty")).arg(float_str(o["f_qty"].toDouble(), 2)));
        p.br();
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
    }
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);
    p.setFontSize(20);
    p.ltext(tr("Printer: ") + printer, 0);
    p.br();
    p.ltext(tr("Storage: ") + storages.toList().join(","), 0);

    p.print(printer, QPrinter::Custom);
}
