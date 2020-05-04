#include "c5shoporder.h"
#include "c5database.h"
#include "printreceipt.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5message.h"
#include "printtaxn.h"

C5ShopOrder::C5ShopOrder()
{

}

void C5ShopOrder::setPartner(int partnerCode, const QString &partnerName)
{
    fPartnerCode = partnerCode;
    fPartnerName = partnerName;
}

void C5ShopOrder::setDiscount(int customer, int cardid, int cardmode, double cardvalue)
{
    fCustomerId = customer;
    fCardId = cardid;
    fCardMode = cardmode;
    fCardValue = cardvalue;
}

void C5ShopOrder::setParams(const QDate &dateOpen, const QTime &timeOpen, int saletype)
{
    fDateOpen = dateOpen;
    fTimeOpen = timeOpen;
    fSaleType = saletype;
}

bool C5ShopOrder::write(double total, double card, double discount, bool tax, QList<IGoods> goods)
{
    C5Database db(__c5config.dbParams());
    C5Database dblog(__c5config.dbParams());
    db.startTransaction();
    C5StoreDraftWriter dw(db);
    if (total < 0.0001) {
        return returnFalse(tr("Negative amount"), db);
    }
    if (!tax) {
        if (card > 0.001) {
            return returnFalse(tr("You cannot use this option with card payment mode."), db);
        }
    }

    if (goods.count() == 0) {
        return returnFalse(tr("Empty order"), db);
    }

    if (__c5config.cashId() == 0) {
        return returnFalse(tr("Cashdesk for cash not defined"), db);
    }
    if (__c5config.nocashId() == 0) {
        return returnFalse(tr("Cashdesk for card not defined"), db);
    }
    if (C5Message::question(tr("Write order?")) != QDialog::Accepted) {
        return false;
    }

    QString headerPrefix;
    int headerId;
    if (!dw.hallId(headerPrefix, headerId, __c5config.defaultHall().toInt())) {
        return returnFalse(dw.fErrorMsg, db);
    }
    QString oheaderid;
    if (!dw.writeOHeader(oheaderid, headerId, headerPrefix, ORDER_STATE_CLOSE, __c5config.defaultHall().toInt(), __c5config.defaultTable(), fDateOpen, QDate::currentDate(), QDate::currentDate(), fTimeOpen, QTime::currentTime(), __userid, "", 1, total, (total - card), card, 0, 0, 0, 0, discount, 0, fCardValue, 0, 0, 1, 2, fSaleType, fPartnerCode)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    if (fCustomerId > 0) {
        if (!dw.writeBHistory(oheaderid, fCardMode, fCardId, fCardValue, 0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    if (tax) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
        for (int i = 0; i < goods.count(); i++) {
            IGoods &g = goods[i];
            pt.addGoods(g.taxDept, //dep
                        g.taxAdg, //adg
                        QString::number(g.goodsId), //goods id
                        g.goodsName, //name
                        g.goodsPrice, //price
                        g.goodsQty, //qty
                        fCardValue * 100); //discount
        }
        QString jsonIn, jsonOut, err;
        int result = 0;
        result = pt.makeJsonAndPrint(card, 0, jsonIn, jsonOut, err);

        dblog[":f_id"] = db.uuid();
        dblog[":f_order"] = oheaderid;
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
            db[":f_id"] = oheaderid;
            db.exec("delete from o_tax where f_id=:f_id");
            db[":f_id"] = oheaderid;
            db[":f_dept"] = C5Config::taxDept();
            db[":f_firmname"] = firm;
            db[":f_address"] = address;
            db[":f_devnum"] = devnum;
            db[":f_serial"] = sn;
            db[":f_fiscal"] = fiscal;
            db[":f_receiptnumber"] = rseq;
            db[":f_hvhh"] = hvhh;
            db[":f_fiscalmode"] = tr("(F)");
            db[":f_time"] = time;
            dblog.insert("o_tax", false);
            pt.saveTimeResult(oheaderid, *q);
            delete q;
        } else {
            pt.saveTimeResult("Not saved - " + oheaderid, *q);
            delete q;
            return returnFalse(err + "<br>" + jsonOut + "<br>" + jsonIn, db);
        }
    }

    QString storeDocComment = QString("%1 %2%3").arg(tr("Output of sale")).arg(headerPrefix).arg(headerId);
    QString storeDocId;
    QString storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, __c5config.defaultStore(), true, 0);
    if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, 0)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    if (!dw.writeAHeaderStore(storeDocId, __userid, __userid, "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0)) {
        return returnFalse(dw.fErrorMsg, db);
    }

    for (int i = 0; i < goods.count(); i++) {
        IGoods &g = goods[i];
        QString ogoodsid;
        QString adraftid;
        if (!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), -1, g.goodsId, g.goodsQty, 0, 0, DOC_REASON_SALE, adraftid, i + 1)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeOGoods(ogoodsid, oheaderid, "", __c5config.defaultStore(), g.goodsId, g.goodsQty, g.goodsPrice,  g.goodsTotal, tax ? rseq.toInt() : 0, 1, i + 1, adraftid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    double cash = total - card;
    QString cashdocid, nocashdocid;
    if (cash > 0) {
        int counter = dw.counterAType(DOC_TYPE_CASH);
        if (counter == 0) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, cash, __c5config.cashPrefix() + " " + headerPrefix + QString::number(headerId), 0, 0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderCash(cashdocid, __c5config.cashId(), 0, 1, "", oheaderid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        QString cashUUID;
        if (!dw.writeECash(cashUUID, cashdocid, __c5config.cashId(), 1, __c5config.cashPrefix() + " " + headerPrefix + QString::number(headerId), cash, cashUUID, 1)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }
    if (card > 0) {
        int counter = dw.counterAType(DOC_TYPE_CASH);
        if (counter == 0) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeader(nocashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, card, __c5config.nocashPrefix() + " " + headerPrefix + QString::number(headerId), 0, 0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderCash(nocashdocid, __c5config.nocashId(), 0, 1, "", oheaderid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        QString cashUUID;
        if (!dw.writeECash(cashUUID, nocashdocid, __c5config.nocashId(), 1, __c5config.cashPrefix() + " " + headerPrefix + QString::number(headerId), card, cashUUID, 1)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    QString err;
    if (dw.writeOutput(storeDocId, err)) {
        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_OUTPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, 0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        dw.writeTotalStoreAmount(storeDocId);
    }

    if (fPartnerCode > 0) {
        db[":f_id"] = fPartnerCode;
        db.exec("select f_store, f_cash from c_partners where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt(0) > 0) {
                int partnerStore = db.getInt(0);
                int partnerCash = db.getInt(1);
                QString comment = tr("Store input") + " " + fPartnerName;
                QString pstoredoc;
                storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, partnerStore, true, 0);
                if (!dw.writeAHeader(pstoredoc, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, tr("Store input") + " " + fPartnerName, 0, 0)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                if (!dw.writeAHeaderStore(pstoredoc, __userid, __userid, "", QDate(), partnerStore, 0, 1, "", 0, 0)) {
                    return returnFalse(dw.fErrorMsg, db);
                }

                for (int i = 0; i < goods.count(); i++) {
                    IGoods &g = goods[i];
                    QString adraftid;
                    if (!dw.writeAStoreDraft(adraftid, pstoredoc, partnerStore, 1, g.goodsId, g.goodsQty, g.goodsPrice, g.goodsTotal, DOC_REASON_INPUT, adraftid, i + 1)) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                }
                if (dw.writeInput(pstoredoc, err)) {
                    int counter = dw.counterAType(DOC_TYPE_CASH);
                    if (!dw.writeAHeader(pstoredoc, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), fPartnerCode, total, comment, 0, 0)) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                    QString partnerCashDoc;
                    if (!dw.writeAHeader(partnerCashDoc, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), fPartnerCode, total, comment, 0, 0)) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                    if (!dw.writeAHeaderCash(partnerCashDoc, partnerCash, 0, 1, partnerCashDoc, "")) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                    QString cashUUID;
                    if (!dw.writeECash(cashUUID, partnerCashDoc, partnerCash, -1, comment, total, cashUUID, 1)) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                    if (!dw.writeAHeaderStore(pstoredoc, __userid, __userid, "", QDate(), partnerStore, 0, 1, partnerCashDoc, 0, 0)) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                }
                dw.writeAHeader2ShopStore(pstoredoc, partnerStore, 0);
            }
        }
    }

    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(oheaderid, db);
    }

    db.commit();
    return true;
}

bool C5ShopOrder::returnFalse(const QString &msg, C5Database &db)
{
    db.rollback();
    db.close();
    C5Message::error(msg);
    return false;
}
