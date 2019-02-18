#include "c5datasyncthread.h"
#include "c5database.h"

C5DataSyncThread::C5DataSyncThread(const QStringList &dbParamsSrc, const QStringList &dbParamsDst, int infoRow, QObject *parent) :
    QThread(parent),
    fDbParamsSrc(dbParamsSrc),
    fDbParamsDst(dbParamsDst),
    fInfoRow(infoRow)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void C5DataSyncThread::runWithDate(const QDate &d1, const QDate &d2)
{
    fDate1 = d1;
    fDate2 = d2;
    start();
}

C5DataSyncThread::~C5DataSyncThread()
{

}

void C5DataSyncThread::run()
{
    emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, tr("Started"));

    C5Database dbSrc(fDbParamsSrc);
    C5Database dbDst(fDbParamsDst);

    int totalRows = 0;
    dbSrc[":f_datecash1"] = fDate1;
    dbSrc[":f_datecash2"] = fDate2;
    if (!dbSrc.exec("select count(f_id) from o_header where f_datecash between :f_datecash1 and :f_datecash2")) {
        emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, dbSrc.fLastError);
        return;
    }
    dbSrc.nextRow();
    totalRows += dbSrc.getInt(0);
    dbSrc[":f_datecash1"] = fDate1;
    dbSrc[":f_datecash2"] = fDate2;
    if (!dbSrc.exec("select count(f_id) from o_goods where f_header in (select f_id from o_header where f_datecash between :f_datecash1 and :f_datecash2)")) {
        emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, dbSrc.fLastError);
        return;
    }
    dbSrc.nextRow();
    totalRows += dbSrc.getInt(0);
    emit progressBarMax(totalRows);

    int progress = 0;
    dbSrc[":f_datecash1"] = fDate1;
    dbSrc[":f_datecash2"] = fDate2;
    if (!dbSrc.exec("select * from o_header where f_datecash between :f_datecash1 and :f_datecash2")) {
        emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, dbSrc.fLastError);
        return;
    }
    while (dbSrc.nextRow()) {
        dbDst.setBindValues(dbSrc.getBindValues());
        dbDst.insert("o_header", false);
        progress++;
        if (progress % 50 == 0) {
            emit progressBarValue(progress);
        }
    }
    dbSrc[":f_datecash1"] = fDate1;
    dbSrc[":f_datecash2"] = fDate2;
    if (!dbSrc.exec("select * from o_goods where f_header in (select f_id from o_header where f_datecash between :f_datecash1 and :f_datecash2)")) {
        emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, dbSrc.fLastError);
        return;
    }
    while (dbSrc.nextRow()) {
        dbDst.setBindValues(dbSrc.getBindValues());
        dbDst.insert("o_goods", false);
        progress++;
        if (progress % 50 == 0) {
            emit progressBarValue(progress);
        }
    }
    emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, tr("Finished"));
}
