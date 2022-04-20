#include "rawbalancehistory.h"
#include "databaseconnectionmanager.h"
#include "database.h"
#include "logwriter.h"

struct BalanceRecord {
    double date;
    QString comment;
    double amount;
};

RawBalanceHistory::RawBalanceHistory(SslSocket *s) :
    Raw(s)
{

}

RawBalanceHistory::~RawBalanceHistory()
{
    qDebug() << "RawBalanceHistory";
}

int RawBalanceHistory::run(const QByteArray &d)
{
    quint8 h;
    readUByte(h, d);
    switch (h) {
    case 1:
        balanceAmountTotal();
        break;
    case 2:
        double db1, db2;
        readDouble(db1, d);
        readDouble(db2, d);
        balanceDetails(QDateTime::fromMSecsSinceEpoch(db1), QDateTime::fromMSecsSinceEpoch(db2));
        break;
    }
    return 0;
}

void RawBalanceHistory::balanceAmountTotal()
{
    double amount = 0;
    int user = fMapTokenUser[fMapSocketToken[fSocket]];
    qDebug() << user << "request balance amount";
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        if (!db.exec("select sum(finout*famount) as famount from acc_balance ")) {
            LogWriter::write(LogWriterLevel::errors, "RawBalanceHistory::balanceAmount", db.lastDbError());
        }
        if (db.next()) {
            amount = db.doubleValue("famount");
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawBalanceHistory::balanceAmount", db.lastDbError());
    }
    quint8 reply = 1;
    putUByte(reply);
    putDouble(amount);
}

void RawBalanceHistory::balanceDetails(const QDateTime &d1, const QDateTime &d2)
{
    int user = fMapTokenUser[fMapSocketToken[fSocket]];
    qDebug() << user << "requested detailed balance" << d1 << d2;
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db) == false) {
        LogWriter::write(LogWriterLevel::errors, "RawBalanceHistory::balanceAmount", db.lastDbError());
        putUByte(0);
        return;
    }
    putUByte(1);
    double finalamount = 0;
    QVector<BalanceRecord> data;
    db[":fdate"] = d1;
    db.exec("select sum(finout*famount) as famount from acc_balance where fdate<:fdate");
    if (db.next() == false) {
        LogWriter::write(LogWriterLevel::errors, "RawBalanceHistory::balanceAmount", db.lastDbError());
        putUByte(0);
        return;
    }
    BalanceRecord br;
    br.date = d1.toMSecsSinceEpoch();
    br.comment = tr("Starting balance");
    br.amount = db.doubleValue("famount");
    data.append(br);
    finalamount = br.amount;
    db[":fdate1"] = d1;
    db[":fdate2"] = d2;
    db.exec("select fdate, fcomment, famount*finout as famount from acc_balance where fdate between :fdate1 and :fdate2 order by fdate");
    while (db.next()) {
        br.date = db.dateTimeValue("fdate").toMSecsSinceEpoch();
        br.comment = db.stringValue("fcomment");
        br.amount = db.doubleValue("famount");
        data.append(br);
        finalamount += br.amount;
    }
    br.date = d2.toMSecsSinceEpoch();
    br.comment = tr("Ending balance");
    br.amount = finalamount;
    data.append(br);
    putUInt(data.count());
    for (const auto &b: data) {
        putDouble(b.date);
        putString(b.comment);
        putDouble(b.amount);
    }
}
