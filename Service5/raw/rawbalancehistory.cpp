#include "rawbalancehistory.h"
#include "databaseconnectionmanager.h"
#include "database.h"
#include "logwriter.h"

RawBalanceHistory::RawBalanceHistory(SslSocket *s) :
    Raw(s)
{

}

RawBalanceHistory::~RawBalanceHistory()
{
    qDebug() << "RawBalanceHistory";
}

void RawBalanceHistory::run(const QByteArray &d)
{
    quint8 h = readUByte(d);
    switch (h) {
    case 1:
        balanceAmountTotal();
        break;
    case 2:
        break;
    }
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
