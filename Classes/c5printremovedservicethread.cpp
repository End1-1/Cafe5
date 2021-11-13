#include "c5printremovedservicethread.h"
#include "c5printing.h"
#include "c5utils.h"
#include "c5config.h"
#include "datadriver.h"
#include "c5database.h"
#include <QApplication>

C5PrintRemovedServiceThread::C5PrintRemovedServiceThread(const QString &id, QObject *parent) :
    QThread(parent),
    fId(id)
{
    connect(this, &C5PrintRemovedServiceThread::finished, this, &C5PrintRemovedServiceThread::deleteLater);
}

void C5PrintRemovedServiceThread::run()
{
    C5Database db(__c5config.dbParams());
    db.exec("select f_alias, f_printer from d_print_aliases");
    while (db.nextRow()) {
        fPrinterAliases[db.getString("f_alias")] = db.getString("f_printer");
    }
    db[":f_id"] = fId;
    db.exec("select * from o_body where f_id=:f_id");
    if (db.nextRow()) {
        db.rowToMap(fBody);
    }
    db[":f_id"] = fBody["f_header"];
    db.exec("select * from o_header where f_id=:f_id");
    if (db.nextRow()) {
        db.rowToMap(fHeader);
    }
    if (fBody["f_print1"].toString().length() > 0) {
        print(fBody["f_print1"].toString());
    }
    if (fBody["f_print2"].toString().length() > 0) {
        print(fBody["f_print2"].toString());
    }
}

void C5PrintRemovedServiceThread::print(QString printName)
{
    QFont font(qApp->font());
    font.setPointSize(18);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.image(":/cancel.png", Qt::AlignHCenter);
    p.br();
    p.br();
    p.ctext(tr("REMOVED FROM ORDER"));
    p.br();
    p.ctext(dbbodystate->name(fBody["f_state"].toInt()));
    p.br();
    p.ltext(tr("Comment"), 0);
    p.br();
    p.ltext(fBody["f_removereason"].toString(), 50);
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(dbtable->name(fHeader["f_table"].toInt()));
    p.br();
    p.ltext(tr("Order no"), 0);
    p.rtext(QString("%1%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_hallid"].toString()));
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(dbuser->fullName(fBody["f_removeuser"].toInt()));
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);


    p.ltext(dbstore->name(fBody["f_store"].toInt()), 0);
    p.br();
    p.setFontSize(26);
    p.ltext(dbdish->name(fBody["f_dish"].toInt()), 0);
    p.br();
    p.ctext(float_str(fBody["f_qty1"].toDouble(), 2));
    p.br();
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);

    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);
    p.setFontSize(8);
    p.ltext(tr("Printer: ") + " " + printName, 0);
    p.br();

    if (fPrinterAliases.contains(printName)) {
        printName = fPrinterAliases[printName];
    }
    p.print(printName, QPrinter::Custom);
}
