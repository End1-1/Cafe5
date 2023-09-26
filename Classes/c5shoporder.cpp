#include "c5shoporder.h"
#include "c5database.h"
#include "printreceipt.h"
#include "printreceiptgroup.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5message.h"
#include "c5logsystem.h"
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
    C5Database db(__c5config.dbParams());
    C5Database dblog(__c5config.dbParams());
    db.startTransaction();
    C5StoreDraftWriter dw(db);

    if (fOGoods.count() == 0) {
        return returnFalse(tr("Empty order"), db);
    }

    if (__c5config.cashId() == 0) {
        return returnFalse(tr("Cashdesk for cash not defined"), db);
    }
    if (__c5config.nocashId() == 0) {
        return returnFalse(tr("Cashdesk for card not defined"), db);
    }

    QString err;
    if (!dw.hallId(fOHeader.prefix, fOHeader.hallId, fOHeader.hall)) {
        return returnFalse(dw.fErrorMsg, db);
    }

    fOHeader.id = "";
    if (!fOHeader.write(db, err)) {
        return returnFalse(err, db);
    }
    if (!dw.writeOHeaderOptions(fOHeader._id(), 0, 0, 0, fOHeader.currency)) {
        return returnFalse(dw.fErrorMsg, db);
    }

    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    if (fOHeader.partner > 0) {
        if (!fBHistory.write(db, err)) {
            return returnFalse(err, db);
        }
    }

    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.price = g.total / g.qty;
    }

    if (C5Config::taxIP().isEmpty()) {
        fOHeader._printFiscal = false;
    }
    if (fOHeader._printFiscal) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                     fOHeader.hasIdram() ? "true" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsonIn, jsonOut, err;
        int result = 0;
        if (fOHeader.partner > 0) {
            pt.fPartnerTin = CPartners().queryRecordOfId(db, fOHeader.partner).taxCode;
        }
        if (!fWriteAdvance) {
            for (int i = 0; i < fOGoods.count(); i++) {
                OGoods &g = fOGoods[i];
                pt.addGoods(g.taxDept, //dep
                            g.adgCode, //adg
                            QString::number(g.goods), //goods id
                            g._goodsName, //name
                            g.price, //price
                            g.qty, //qty
                            fBHistory.value * 100); //discount
            }
            result = pt.makeJsonAndPrint(fOHeader.amountCard + fOHeader.amountIdram + fOHeader.amountTelcell, fOHeader.amountPrepaid, jsonIn, jsonOut, err);
        } else {
            result = pt.printAdvanceJson(fOHeader.amountCash, fOHeader.amountCard, jsonIn, jsonOut, err);
        }

        dblog[":f_id"] = db.uuid();
        dblog[":f_order"] = fOHeader.id;
        dblog[":f_date"] = QDate::currentDate();
        dblog[":f_time"] = QTime::currentTime();
        dblog[":f_in"] = jsonIn;
        dblog[":f_out"] = jsonOut;
        dblog[":f_err"] = err;
        dblog[":f_result"] = result;
        dblog.insert("o_tax_log", false);
        QSqlQuery *q = new QSqlQuery(dblog.fDb);
        if (result == pt_err_ok) {
            PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
            dblog[":f_id"] = fOHeader.id;
            db.exec("delete from o_tax where f_id=:f_id");
            dblog[":f_id"] = fOHeader.id;
            dblog[":f_dept"] = C5Config::taxDept();
            dblog[":f_firmname"] = firm;
            dblog[":f_address"] = address;
            dblog[":f_devnum"] = devnum;
            dblog[":f_serial"] = sn;
            dblog[":f_fiscal"] = fiscal;
            dblog[":f_receiptnumber"] = rseq;
            dblog[":f_hvhh"] = hvhh;
            dblog[":f_fiscalmode"] = tr("(F)");
            dblog[":f_time"] = time;
            dblog.insert("o_tax", false);
            pt.saveTimeResult(fOHeader._id(), *q);
            delete q;
        } else {
            pt.saveTimeResult("Not saved - " + fOHeader._id(), *q);
            delete q;
            return returnFalse(err + "<br>" + jsonOut + "<br>" + jsonIn, db);
        }
    }

    bool needStoreDoc = false;
    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.header = fOHeader._id();
        g.tax = rseq.toInt();
        if (!g.isService) {
            needStoreDoc = true;
        }
    }

    QString storeDocComment = QString("%1 %2").arg(tr("Output of sale"), fOHeader.humanId());
    QString storeDocId;
    QString storedocUserNum;
    if (needStoreDoc) {
        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, __c5config.defaultStore(), true, 0);
        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, fOHeader.staff, QDate::currentDate(),
                             QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, fOHeader.currency)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderStore(storeDocId, fOHeader.staff, fOHeader.staff, "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0, fOHeader._id())) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.header = fOHeader._id();
        if (!g.isService) {
            if (!dw.writeAStoreDraft(g.storeRec, storeDocId, __c5config.defaultStore(), -1, g.goods, g.qty,
                                     0, 0, DOC_REASON_SALE, g.storeRec, i + 1, "")) {
                return returnFalse(dw.fErrorMsg, db);
            }
        }

        if (!g.write(db, err)) {
            return returnFalse(err, db);
        }
    }

    if (needStoreDoc) {
        if (dw.writeOutput(storeDocId, err)) {
            if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_OUTPUT, fOHeader.staff,
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, fOHeader.currency)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            dw.writeTotalStoreAmount(storeDocId);
        }
    }

    writeCash(db, fOHeader.amountCash, __c5config.cashId());
    writeCash(db, fOHeader.amountCard, __c5config.nocashId());
    writeCash(db, fOHeader.amountIdram, __c5config.nocashId());
    writeCash(db, fOHeader.amountTelcell, __c5config.nocashId());
    if (fOHeader.amountBank > 0.001) {
        BClientDebts b;
        b.source = BCLIENTDEBTS_SOURCE_SALE;
        b.date = fOHeader.dateCash;
        b.costumer = fOHeader.partner;
        b.order = fOHeader._id();
        b.amount = -1 * fOHeader.amountBank;
        b.currency = fOHeader.currency;
        b.write(db, err);
    }
    if (fOHeader.amountDebt > 0.001) {
        BClientDebts b;
        b.source = BCLIENTDEBTS_SOURCE_SALE;
        b.date = fOHeader.dateCash;
        b.costumer = fOHeader.partner;
        b.order = fOHeader._id();
        b.amount = -1 * fOHeader.amountDebt;
        b.currency = fOHeader.currency;
        b.write(db, err);
    }

    if (fOHeader.partner > 0 && needStoreDoc) {
        OutputOfHeader ooh;
        ooh.make(db, fOHeader._id());
    }


    db.commit();
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;
        switch (C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;
            if (SelectPrinters::selectPrinters(p1, p2)) {
                if (p1) {
                    p.print(fOHeader._id(), db, 1);
                }
                if (p2) {
                    p.print(fOHeader._id(), db, 2);
                }
            }
            break;
        }
        case 2:
            p.print2(fOHeader._id(), db);
            break;
        case 3:
            p.print3(fOHeader._id(), db);
            break;
        default:
            break;
        }
    }

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
    C5LogSystem::writeEvent(msg);
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
