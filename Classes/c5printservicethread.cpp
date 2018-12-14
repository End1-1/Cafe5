#include "c5printservicethread.h"
#include "c5printing.h"
#include "c5utils.h"
#include <QApplication>

C5PrintServiceThread::C5PrintServiceThread(const QJsonObject &header, const QJsonArray &body, QObject *parent) :
    QThread(parent)
{
    fHeader = header;
    fBody = body;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

C5PrintServiceThread::~C5PrintServiceThread()
{
    qDebug();
}

void C5PrintServiceThread::run()
{
    QSet<QString> fPrint1, fPrint2;
    for (int i = 0; i < fBody.count(); i++) {
        QJsonObject jb = fBody[i].toObject();
        if (jb["f_qtyprint"].toString().toDouble() > 0.01) {
            if (jb["f_print1"].toString().length() > 0) {
                fPrint1 << jb["f_print1"].toString();
            }
            if (jb["f_print1"].toString() != jb["f_print2"].toString()) {
                if (jb["f_print2"].toString().length() > 0) {
                    fPrint2 << jb["f_print2"].toString();
                }
            }
        }
    }

    foreach (QString s, fPrint1) {
        print(s, "f_print1");
    }

    foreach (QString s, fPrint2) {
        print(s, "f_print2");
    }
}

void C5PrintServiceThread::print(const QString &printer, const QString &side)
{
    QFont font(qApp->font());
    font.setPointSize(30);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.ctext(tr("New order"));
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
    for (int i = 0; i < fBody.count(); i++) {
        QJsonObject o = fBody[i].toObject();
        if (o["f_qtyprint"].toString().toDouble() < 0.01) {
            continue;
        }
        if (o[side].toString() != printer) {
            continue;
        }
        storages << o["f_storename"].toString();
        p.ltext(o["f_name"].toString(), 0);
        p.br();
        if (o["f_comment"].toString().length() > 0) {
            p.ltext(o["f_comment"].toString(), 0);
            p.br();
        }
        p.ctext(o["f_qtyprint"].toString());
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
