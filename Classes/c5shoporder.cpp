#include "c5shoporder.h"
#include "c5database.h"
#include "printreceipt.h"
#include "printreceiptgroup.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5message.h"
#include "selectprinters.h"
#include "printtaxn.h"
#include "cpartners.h"
#include "ecash.h"
#include "bclientdebts.h"
#include "aheader.h"
#include "c5user.h"
#include "outputofheader.h"

C5ShopOrder::C5ShopOrder(OHeader &oheader, BHistory &bhistory, QVector<OGoods> &ogoods) :
    fOHeader(oheader),
    fBHistory(bhistory),
    fOGoods(ogoods)
{
    fWriteAdvance = false;
}

bool C5ShopOrder::write()
{
    //    C5Database db(__c5config.dbParams());
    //    C5Database dblog(__c5config.dbParams());
    //    db.startTransaction();
    //    QString err;
    //    if (!dw.hallId(fOHeader.prefix, fOHeader.hallId, fOHeader.hall)) {
    //        return returnFalse(dw.fErrorMsg, db);
    //    }
    //    fOHeader.id = "";
    //    if (!fOHeader.write(db, err)) {
    //        return returnFalse(err, db);
    //    }
    //    if (!dw.writeOHeaderOptions(fOHeader._id(), 0, 0, 0, fOHeader.currency)) {
    //        return returnFalse(dw.fErrorMsg, db);
    //    }
    //    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    //    if (fOHeader.partner > 0) {
    //        if (!fBHistory.write(db, err)) {
    //            return returnFalse(err, db);
    //        }
    //    }
    //    for (int i = 0; i < fOGoods.count(); i++) {
    //        OGoods &g = fOGoods[i];
    //        g.price = g.total / (g.qty / g._qtybox);
    //    }
    //    bool needStoreDoc = false;
    //    for (int i = 0; i < fOGoods.count(); i++) {
    //        OGoods &g = fOGoods[i];
    //        g.header = fOHeader._id();
    //        g.tax = rseq.toInt();
    //        if (!g.isService) {
    //            needStoreDoc = true;
    //        }
    //    }
    //    QString storeDocComment = QString("%1 %2").arg(tr("Output of sale"), fOHeader.humanId());
    //    QString storeDocId;
    //    QString storedocUserNum;
    //    if (needStoreDoc) {
    //        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, __c5config.defaultStore(), true, 0);
    //        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, fOHeader.staff,
    //                             QDate::currentDate(),
    //                             QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, fOHeader.currency)) {
    //            return returnFalse(dw.fErrorMsg, db);
    //        }
    //        if (!dw.writeAHeaderStore(storeDocId, fOHeader.staff, fOHeader.staff, "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0, fOHeader._id())) {
    //            return returnFalse(dw.fErrorMsg, db);
    //        }
    //    }
    //    for (int i = 0; i < fOGoods.count(); i++) {
    //        OGoods &g = fOGoods[i];
    //        g.header = fOHeader._id();
    //        if (!g.isService) {
    //            if (!dw.writeAStoreDraft(g.storeRec, storeDocId, __c5config.defaultStore(), -1, g.goods, g.qty / g._qtybox,
    //                                     0, 0, DOC_REASON_SALE, g.storeRec, i + 1, "")) {
    //                return returnFalse(dw.fErrorMsg, db);
    //            }
    //        }
    //        g.qty = g.qty / g._qtybox;
    //        if (!g.write(db, err)) {
    //            return returnFalse(err, db);
    //        }
    //    }
    //    if (needStoreDoc) {
    //        if (dw.writeOutput(storeDocId, err)) {
    //            if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_OUTPUT, fOHeader.staff,
    //                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment,
    //                                 0, fOHeader.currency)) {
    //                return returnFalse(dw.fErrorMsg, db);
    //            }
    //            dw.writeTotalStoreAmount(storeDocId);
    //        }
    //    }
    //    writeCash(db, fOHeader.amountCash, __c5config.cashId());
    //    writeCash(db, fOHeader.amountCard, __c5config.nocashId());
    //    writeCash(db, fOHeader.amountIdram, __c5config.nocashId());
    //    writeCash(db, fOHeader.amountTelcell, __c5config.nocashId());
    //    if (fOHeader.amountBank > 0.001) {
    //        BClientDebts b;
    //        b.source = BCLIENTDEBTS_SOURCE_SALE;
    //        b.date = fOHeader.dateCash;
    //        b.costumer = fOHeader.partner;
    //        b.order = fOHeader._id();
    //        b.amount = -1 * fOHeader.amountBank;
    //        b.currency = fOHeader.currency;
    //        b.write(db, err);
    //    }
    //    if (fOHeader.amountDebt > 0.001) {
    //        BClientDebts b;
    //        b.source = BCLIENTDEBTS_SOURCE_SALE;
    //        b.date = fOHeader.dateCash;
    //        b.costumer = fOHeader.partner;
    //        b.order = fOHeader._id();
    //        b.amount = -1 * fOHeader.amountDebt;
    //        b.currency = fOHeader.currency;
    //        b.write(db, err);
    //    }
    return true;
}

bool C5ShopOrder::writeFlags(int f1, int f2, int f3, int f4, int f5)
{
    C5Database db(__c5config.dbParams());
    C5StoreDraftWriter dw(db);
    return dw.writeOHeaderFlags(fOHeader._id(), f1, f2, f3, f4, f5);
}

bool C5ShopOrder::returnFalse(const QString &msg, C5Database &db)
{
    db.rollback();
    db.close();
    C5Message::error(msg);
    return false;
}

bool C5ShopOrder::writeCash(C5Database &db, double value, int cash)
{
    if (value < 0.001) {
        return true;
    }
    QString err;
    AHeader aheader;
    aheader.userId = fOHeader.humanId();
    aheader.date = fOHeader.dateCash;
    aheader.dateCreate = fOHeader.dateClose;
    aheader.timeCreate = fOHeader.timeClose;
    aheader.comment = QString("%1 %2").arg(tr("Input of sale"), aheader.userId);
    aheader.state = DOC_STATE_SAVED;
    aheader.type = DOC_TYPE_CASH;
    aheader.operator_ = fOHeader.cashier;
    aheader.amount = value;
    aheader.currency = fOHeader.currency;
    aheader.write(db, err);
    db[":f_id"] = aheader.id;
    db[":f_cashin"] = cash;
    db[":f_cashout"] = 0;
    db[":f_oheader"] = fOHeader.id;
    db[":f_related"] = 1;
    db.insert("a_header_cash", false);
    ECash ecash;
    ecash.header = aheader.id;
    ecash.cash = cash;
    ecash.sign = 1;
    ecash.row = 1;
    ecash.amount = value;
    ecash.remarks = aheader.comment;
    ecash.write(db, err);
    return true;
}
