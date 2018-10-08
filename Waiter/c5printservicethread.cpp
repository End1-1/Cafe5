#include "c5printservicethread.h"
#include "c5printing.h"
#include "c5utils.h"
#include <QDebug>
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
    qDebug() << "End of C5PrintSericeThread";
}

void C5PrintServiceThread::run()
{
    QFont font(qApp->font());
    C5Printing p("local");
    p.setSceneParams(70, 280, QPrinter::Portrait);
    p.setFont(font);

    p.line(35, 0, 35, 100);
    p.ctext(tr("New order"));
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(fHeader["f_table"].toString());
    p.br();
    p.ltext(tr("Order no"), 0);
    p.rtext(QString("%1-%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_id"].toString()));
    p.br();
    p.ltext(tr("Date"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Time"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(fHeader["f_staff"].toString());
    p.br();
    p.br();
    p.line(0, p.fTop, 70, p.fTop);
    p.br(2);
    p.line(0, p.fTop, 70, p.fTop);
    p.print();
}
