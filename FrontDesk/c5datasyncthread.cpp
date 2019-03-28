#include "c5datasyncthread.h"
#include "c5database.h"
#include "c5utils.h"

C5DataSyncThread::C5DataSyncThread(const QStringList &dbParamsSrc, const QStringList &dbParamsDst, int infoRow, QObject *parent) :
    QThread(parent),
    fDbParamsSrc(dbParamsSrc),
    fDbParamsDst(dbParamsDst),
    fInfoRow(infoRow)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    fRunMode = rmNone;
}

void C5DataSyncThread::runWithDate(const QDate &d1, const QDate &d2, bool removeFromSource)
{
    fDate1 = d1;
    fDate2 = d2;
    fRemoveFromSrc = removeFromSource;
    fRunMode = rmDownloadSales;
    start();
}

void C5DataSyncThread::runUploadData()
{
    fRunMode = rmUploadChanges;
    start();
}

C5DataSyncThread::~C5DataSyncThread()
{

}

void C5DataSyncThread::run()
{
    switch (fRunMode) {
    case rmNone:
        break;
    case rmDownloadSales:
        downloadData();
        break;
    case rmUploadChanges:
        uploadData();
        break;
    }
}

void C5DataSyncThread::downloadData()
{
    emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, tr("Started"));

    QStringList idHeader;
    QStringList idGoods;
    bool noError = true;

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
        idHeader << dbSrc.getString("f_id");
        dbDst[":f_id"] = dbSrc.getString("f_id");
        dbDst.exec("delete from o_header where f_id=:f_id");
        dbDst.setBindValues(dbSrc.getBindValues());
        noError &= dbDst.insert("o_header", false) > 0;
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
    int progressValue = 0;
    while (dbSrc.nextRow()) {
        idGoods << dbSrc.getString("f_id");
        dbDst[":f_id"] = dbSrc.getString("f_id");
        dbDst.exec("delete from o_goods where f_id=:f_id");
        dbDst.setBindValues(dbSrc.getBindValues());
        noError &= dbDst.insert("o_goods", false) > 0;
        progress++;
        progressValue++;
        if (progress % 50 == 0) {
            emit progressBarValue(progressValue);
            progressValue = 0;
        }
    }
    if (fRemoveFromSrc && noError) {
        foreach (QString s, idGoods) {
            dbSrc[":f_id"] = s;
            dbSrc.exec("delete from o_goods where f_Id=:f_id");
        }
        foreach (QString s, idHeader) {
            dbSrc[":f_id"] = s;
            dbSrc.exec("delete from o_header where f_Id=:f_id");
        }
    }
    emit statusUpdated(fDbParamsSrc.at(4), fInfoRow, tr("Finished"));
}

void C5DataSyncThread::uploadData()
{
    QDateTime tc = QDateTime::currentDateTime();
    QStringList tables;

    C5Database dbSrc(fDbParamsSrc);
    C5Database dbDst(fDbParamsDst);

    int totalRows = 0;
    dbSrc[":f_host"] = fDbParamsDst.at(0);
    dbSrc[":f_db"] = fDbParamsDst.at(1);
    dbSrc.exec("select a from s_db where f_host=:f_host and f_db=:f_db");
    if (!dbSrc.nextRow()) {
        emit statusUploadUpdated(fInfoRow, 4, tr("No such database.") + " " + fDbParamsDst.at(0) + ":" + fDbParamsDst.at(1));
        return;
    }
    QDateTime ts = dbSrc.getDateTime(0);
    if (!ts.isValid()) {
        ts = QDateTime::fromString("01/01/2019 00:00:00", FORMAT_DATETIME_TO_STR);
    }
    emit statusUploadUpdated(fInfoRow, 1, ts.toString(FORMAT_DATETIME_TO_STR));

    dbSrc.exec("select f_name from s_sync_tables order by f_id");
    while (dbSrc.nextRow()) {
        tables << dbSrc.getString(0);
    }
    foreach (QString t, tables) {
        dbSrc[":a1"] = ts;
        dbSrc[":a2"] = tc;
        dbSrc.exec(QString("select count(f_id) from %1 where a between :a1 and :a2").arg(t));
        dbSrc.nextRow();
        totalRows += dbSrc.getInt(0);
    }
    emit statusUploadUpdated(fInfoRow, 2, QString::number(totalRows));
    emit progressBarMax(totalRows);

    int currentRow = 0;
    int progressValue = 0;
    bool err = false;
    foreach (QString t, tables) {
        dbSrc[":a1"] = ts;
        dbSrc[":a2"] = tc;
        dbSrc.exec(QString("select * from %1 where a between :a1 and :a2").arg(t));
        while (dbSrc.nextRow()) {
            currentRow++;
            progressValue++;
            QMap<QString, QVariant> values = dbSrc.getBindValues();
            values.remove(":a");
            dbDst[":f_id"] = values[":f_id"];
            dbDst.exec("select * from " + t + " where f_id=:f_id");
            bool localErr = false;
            if (dbDst.nextRow()) {
                dbDst.setBindValues(values);
                localErr = !dbDst.update(t, where_id(values[":f_id"].toString()));
            } else {
                dbDst.setBindValues(values);
                localErr = !dbDst.insert(t, false);
            }
            if (localErr) {
                err = true;
                emit errLog(QString("%1:%2  %3")
                            .arg(fDbParamsDst.at(0))
                            .arg(fDbParamsDst.at(1))
                            .arg(dbDst.fLastError));
            }
        }
        if (currentRow % 50 == 0) {
            emit statusUploadUpdated(fInfoRow, 3, QString::number(currentRow));
            emit progressBarValue(progressValue);
            progressValue = 0;
        }
    }

    if (!err) {
        dbSrc[":f_host"] = fDbParamsDst.at(0);
        dbSrc[":f_db"] = fDbParamsDst.at(1);
        dbSrc[":a"] = tc;
        dbSrc.exec("update s_db set a=:a where f_host=:f_host and f_db=:f_db");
    }
    emit statusUploadUpdated(fInfoRow, 4, tr("Finished"));
}
