#include "c5printremovedservicethread.h"
#include "c5printing.h"
#include "c5utils.h"
#include <QApplication>

C5PrintRemovedServiceThread::C5PrintRemovedServiceThread(const QJsonObject &h, const QJsonObject &b, QObject *parent) :
    QThread(parent)
{
    fHeader = h;
    fBody = b;
}

void C5PrintRemovedServiceThread::run()
{
    if (fBody["f_print1"].toString().length() > 0) {
        print(fBody["f_print1"].toString());
    }
    if (fBody["f_print2"].toString().length() > 0) {
        if (fBody["f_print2"].toString() != fBody["f_print1"].toString()) {
            print(fBody["f_print2"].toString());
        }
    }
}

void C5PrintRemovedServiceThread::print(const QString &printName)
{
    QJsonObject o = fBody;
    QFont font(qApp->font());
    font.setPointSize(24);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.image(":/cancel.png", Qt::AlignHCenter);
    p.br();
    p.ctext(tr("REMOVED FROM ORDER"));
    p.br();
    p.ltext(tr("Reason"), 0);
    switch (abs(o["f_state"].toString().toInt())) {
    case DISH_STATE_MISTAKE:
        p.rtext(tr("Mistake"));
        break;
    case DISH_STATE_VOID:
        p.rtext(tr("Void"));
        break;
    default:
        p.rtext(tr("May be, programm error occured"));
        break;
    }
    p.br();
    p.ltext(tr("Comment"), 0);
    p.br();
    p.ltext(o["f_removereason"].toString(), 50);
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(fHeader["f_tablename"].toString());
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
    p.rtext(fHeader["f_currentstaffname"].toString());
    p.br(p.fLineHeight + 2);
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);

    QSet<QString> storages;
    storages << o["f_storename"].toString();
    p.ltext(o["f_name"].toString(), 0);
    p.br();
    p.ctext(float_str(o["f_qty1"].toString().toDouble(), 2));
    p.br();
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);

    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);
    p.setFontSize(8);
    p.ltext(tr("Printer: ") + printName, 0);
    p.br();
    p.ltext(tr("Storage: ") + storages.toList().join(","), 0);

    p.print(printName, QPrinter::Custom);
}
