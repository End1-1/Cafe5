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
#include "c5user.h"

C5ShopOrder::C5ShopOrder(C5User *user)
{
    fUser = user;
    fWriteAdvance = false;
}

void C5ShopOrder::setPayment(double cash, double change, bool debt, int currency)
{
    fCash = cash;
    fChange = change;
    fDebt = debt;
    fCurrency = currency;
}

void C5ShopOrder::setPartner(int partnerCode, const QString &partnerName)
{
    fPartnerCode = partnerCode;
    fPartnerName = partnerName;
}

void C5ShopOrder::setDiscount(int cardid, int cardmode, double cardvalue)
{
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

bool C5ShopOrder::write(double total, double card, double prepaid, double discount, bool tax, QList<IGoods> goods, double fDiscountFactor, int discmode, bool idram, int currencyid)
{
    C5Database db(__c5config.dbParams());
    C5Database dblog(__c5config.dbParams());
    db.startTransaction();
    C5StoreDraftWriter dw(db);
    if (total < 0) {
        return returnFalse(tr("Negative amount"), db);
    }
    /*
    if (!tax) {
        if (card > 0.001) {
            return returnFalse(tr("You cannot use this option with card payment mode."), db);
        }
    }
*/
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
    if (!dw.hallId(headerPrefix, headerId, __c5config.defaultHall())) {
        return returnFalse(dw.fErrorMsg, db);
    }
    fHallId = QString("%1%2").arg(headerPrefix).arg( headerId);
    if (!dw.writeOHeader(fHeader, headerId, headerPrefix, ORDER_STATE_CLOSE, __c5config.defaultHall(),
                         __c5config.defaultTable(), fDateOpen, QDate::currentDate(), QDate::currentDate(), fTimeOpen,
                         QTime::currentTime(), fUser->id(), "", 1,
                         total, (total - card - prepaid), idram ? 0 : card, prepaid, 0, 0, idram ? card : 0,
                         0, discount,  0, fCardValue, 0, 0, 1, 2, fSaleType, fPartnerCode)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    if (!dw.writeOHeaderOptions(fHeader, 0, 0, 0, fCurrency)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    if (!dw.writeAHeader(fHeader, fHallId, DOC_STATE_SAVED, DOC_TYPE_SALE_INPUT, fUser->id(), QDate::currentDate(),
                         QDate::currentDate(), QTime::currentTime(), fPartnerCode, total, "", currencyid)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    if (!dw.writeOPayment(fHeader, fCash, fChange)) {
        return returnFalse(dw.fErrorMsg, db);
    }
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    if (fPartnerCode > 0) {
        if (!dw.writeBHistory(fHeader, fCardMode, fCardId, fCardValue, 0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    if (C5Config::taxIP().isEmpty()) {
        tax = false;
    }
    if (tax) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), idram ? "true" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsonIn, jsonOut, err;
        int result = 0;
        pt.fPartnerTin = fPartnerName;
        if (!fWriteAdvance) {
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
            result = pt.makeJsonAndPrint(card, prepaid, jsonIn, jsonOut, err);
        } else {
            result = pt.printAdvanceJson(total - card, card, jsonIn, jsonOut, err);
        }

        dblog[":f_id"] = db.uuid();
        dblog[":f_order"] = fHeader;
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
            dblog[":f_id"] = fHeader;
            db.exec("delete from o_tax where f_id=:f_id");
            dblog[":f_id"] = fHeader;
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
            pt.saveTimeResult(fHeader, *q);
            delete q;
        } else {
            pt.saveTimeResult("Not saved - " + fHeader, *q);
            delete q;
            return returnFalse(err + "<br>" + jsonOut + "<br>" + jsonIn, db);
        }
    }

    bool needStoreDoc = false;
    bool writeStoreBeforeOutput = false;
    for (int i = 0; i < goods.count(); i++) {
        IGoods &g = goods[i];
        if (!g.isService) {
            needStoreDoc = true;
        }
        if (g.writeStoreDocBeforeOutput) {
            writeStoreBeforeOutput = true;
            needStoreDoc = true;
            break;
        }
    }

    if (writeStoreBeforeOutput) {
        double wsbTotal = 0;
        QString wsbDocId;
        QString wsbDocUserNum;
        QString wsbComment = tr("Input before sale");
        wsbDocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, __c5config.defaultStore(), true, 0);
        if (!dw.writeAHeader(wsbDocId, wsbDocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT, fUser->id(), QDate::currentDate(),
                             QDate::currentDate(), QTime::currentTime(), 0, 0, wsbComment, currencyid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderStore(wsbDocId, fUser->id(), fUser->id(), "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0, fHeader)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        int rownum = 1;
        for (int i = 0; i < goods.count(); i++) {
            IGoods &g = goods[i];
            if (g.writeStoreDocBeforeOutput) {
                QString wsbDraftId;
                if (!dw.writeAStoreDraft(wsbDraftId, wsbDocId, __c5config.defaultStore(), 1, g.storeId, g.goodsQty, g.lastInputPrice, g.lastInputPrice * g.goodsQty, DOC_REASON_INPUT, wsbDraftId, rownum++, "")) {
                    return returnFalse(dw.fErrorMsg, db);
                }
            }
        }
        QString err;
        if (!dw.writeInput(wsbDocId, err)) {
            return returnFalse(err, db);
        }
        if (!dw.writeTotalStoreAmount(wsbDocId)) {
            return returnFalse(dw.fErrorMsg, db);
        }

        QString wsbCashDoc;
        QString wsbCashRowId;
        QString wsbCashUserId = QString("%1").arg(wsbDocUserNum.toInt(), C5Config::docNumDigitsInput(), 10, QChar('0'));
        if (!dw.writeAHeader(wsbCashDoc, wsbCashUserId, DOC_STATE_SAVED, DOC_TYPE_CASH, fUser->id(), QDate::currentDate(),
                             QDate::currentDate(), QTime::currentTime(), 0, wsbTotal, wsbComment, currencyid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderCash(wsbCashDoc, 0, __c5config.cashId(), 1, wsbDocId, "",0)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeECash(wsbCashRowId, wsbCashDoc, __c5config.cashId(), -1, wsbComment, wsbTotal, wsbCashRowId, 1)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderStore(wsbDocId, 0, 0, "", QVariant().toDate(), __c5config.defaultStore(), 0, 0, wsbCashDoc, 0, 0, fHeader)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    QString storeDocComment = QString("%1 %2%3").arg(tr("Output of sale"), headerPrefix, QString::number(headerId));
    QString storeDocId;
    QString storedocUserNum;
    if (needStoreDoc) {
        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, __c5config.defaultStore(), true, 0);
        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, fUser->id(), QDate::currentDate(),
                             QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, currencyid)) {
            return returnFalse(dw.fErrorMsg, db);
        }
        if (!dw.writeAHeaderStore(storeDocId, fUser->id(), fUser->id(), "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0, fHeader)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    for (int i = 0; i < goods.count(); i++) {
        IGoods &g = goods[i];
        QString ogoodsid;
        QString adraftid;
        if (!g.isService) {
            if (!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), -1, g.goodsId, g.goodsQty,
                                     0, 0, DOC_REASON_SALE, adraftid, i + 1, "")) {
                return returnFalse(dw.fErrorMsg, db);
            }
        } else {
            if (g.writeStoreDocBeforeOutput) {
                if (!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), -1, g.goodsId, g.goodsQty,
                                         0, 0, DOC_REASON_SALE, adraftid, i + 1, "")) {
                    return returnFalse(dw.fErrorMsg, db);
                }
            }
        }
        double discamount = 0;
        double discfactor = fDiscountFactor;
        if (fDiscountFactor > 0.0001) {
            if (discmode == CARD_TYPE_DISCOUNT) {
                discfactor = fDiscountFactor;
                discamount = g.goodsPrice * fDiscountFactor;
                g.goodsPrice -= discamount;
                g.goodsTotal = g.goodsPrice * g.goodsQty;
            } else {
                if (fDiscountFactor > g.goodsTotal) {
                    discamount = g.goodsTotal;
                    discfactor = discamount;
                    fDiscountFactor -= g.goodsTotal;
                    g.goodsTotal = 0;
                    g.goodsPrice = 0;
                } else {
                    discamount = fDiscountFactor;
                    discfactor = discamount;
                    g.goodsTotal -= fDiscountFactor;
                    g.goodsPrice = g.goodsTotal / g.goodsQty;
                    fDiscountFactor = 0;
                }
            }
        }
        if (!dw.writeOGoods(ogoodsid, fHeader, "", __c5config.defaultStore(), g.goodsId, g.goodsQty, g.goodsPrice,  g.goodsTotal,
                            tax ? rseq.toInt() : 0, 1, i + 1, adraftid, discamount, discmode, 0, "", discfactor)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    if (fDebt) {
        db[":f_doc"] = fHeader;
        db[":f_dc"] = 1;
        db[":f_amount"] = total;
        db.insert("a_dc", false);
    } else {
        double cash = total - card;
        QString cashdocid, nocashdocid;
        if (cash > 0) {
            int counter = dw.counterAType(DOC_TYPE_CASH);
            if (counter == 0) {
                return returnFalse(dw.fErrorMsg, db);
            }
            if (!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, fUser->id(),
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, cash,
                                 __c5config.cashPrefix() + " " + headerPrefix + QString::number(headerId), currencyid)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            if (!dw.writeAHeaderCash(cashdocid, __c5config.cashId(), 0, 1, "", fHeader, 0)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            QString cashUUID;
            if (!dw.writeECash(cashUUID, cashdocid, __c5config.cashId(), 1, __c5config.cashPrefix() + " " + headerPrefix + QString::number(headerId),
                               cash, cashUUID, 1)) {
                return returnFalse(dw.fErrorMsg, db);
            }
        }
        if (card > 0) {
            int counter = dw.counterAType(DOC_TYPE_CASH);
            if (counter == 0) {
                return returnFalse(dw.fErrorMsg, db);
            }
            if (!dw.writeAHeader(nocashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, fUser->id(),
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, card,
                                 __c5config.nocashPrefix() + " " + headerPrefix + QString::number(headerId), currencyid)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            if (!dw.writeAHeaderCash(nocashdocid, __c5config.nocashId(), 0, 1, "", fHeader, 0)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            QString cashUUID;
            if (!dw.writeECash(cashUUID, nocashdocid, __c5config.nocashId(), 1, __c5config.nocashPrefix() + " " + headerPrefix + QString::number(headerId), card, cashUUID, 1)) {
                return returnFalse(dw.fErrorMsg, db);
            }
        }
    }

    QString err;
    if (needStoreDoc) {
        if (dw.writeOutput(storeDocId, err)) {
            if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_OUTPUT, fUser->id(),
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, currencyid)) {
                return returnFalse(dw.fErrorMsg, db);
            }
            dw.writeTotalStoreAmount(storeDocId);
        }
    }

    if (fPartnerCode > 0 && needStoreDoc) {
        db[":f_id"] = fPartnerCode;
        db.exec("select f_store, f_cash from c_partners where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt(0) > 0) {
                int partnerStore = db.getInt(0);
                int partnerCash = db.getInt(1);
                QString comment = QString("%1 %2, %3%4").arg(tr("Store input"), fPartnerName, headerPrefix, QString::number(headerId));
                QString pstoredoc;
                storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, partnerStore, true, 0);
                if (!dw.writeAHeader(pstoredoc, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT, fUser->id(),
                                     QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                                     tr("Store input") + " " + fPartnerName, currencyid)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                if (!dw.writeAHeaderStore(pstoredoc, fUser->id(), fUser->id(), "", QDate(), partnerStore, 0, 0, "", 0, 0, fHeader)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                // DO NOT UNCOMPLECT THE GOODS WITH UNIT ID THAT EQUAL TO 10
                QString goodsComplect;
                foreach (const IGoods &g, goods) {
                    if (g.unitId == 10) {
                        if (!goodsComplect.isEmpty()) {
                            goodsComplect += ",";
                        }
                        goodsComplect += QString::number(g.goodsId);
                    }
                }

                QList<IGoods> ingoods;
                if (!goodsComplect.isEmpty()) {
                    db[":f_document"] = storeDocId;
                    db.exec("select if (gc.f_base is null, ad.f_goods, gc.f_goods) as f_goods, "
                            "if(gc.f_base is null, ad.f_qty, gc.f_qty*ad.f_qty) as f_qty, "
                            "if(gc.f_base is null, ad.f_price, gpr.f_price2) as f_price, "
                            "if(gc.f_base is null, ad.f_price*ad.f_qty, gpr.f_price2*(gc.f_qty*ad.f_qty)) as f_total "
                            "from a_store_draft ad "
                            "left join c_goods_complectation gc on gc.f_base=ad.f_goods "
                            "left join c_goods g on g.f_id=gc.f_goods "
                            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
                            "where f_document=:f_document and ad.f_goods in (" + goodsComplect + ") ");
                    while (db.nextRow()) {
                        IGoods g;
                        g.goodsId = db.getInt("f_goods");
                        g.goodsQty = db.getDouble("f_qty");
                        g.goodsPrice = db.getDouble("f_price");
                        g.goodsTotal = db.getDouble("f_total");
                        ingoods.append(g);
                    }
                }

                if (goodsComplect.isEmpty()) {
                    goodsComplect = "0";
                }
                db[":f_document"] = storeDocId;
                db.exec("select ad.f_goods, ad.f_qty, ad.f_price, ad.f_price*ad.f_qty as f_total "
                          "from a_store_draft ad "
                          "where f_document=:f_document and ad.f_goods not in (" + goodsComplect + ") ");
                while (db.nextRow()) {
                    IGoods g;
                    g.goodsId = db.getInt("f_goods");
                    g.goodsQty = db.getDouble("f_qty");
                    g.goodsPrice = db.getDouble("f_price");
                    g.goodsTotal = db.getDouble("f_total");
                    ingoods.append(g);
                }

                for (int i = 0; i < ingoods.count(); i++) {
                    IGoods &g = ingoods[i];
                    QString adraftid;
                    if (!dw.writeAStoreDraft(adraftid, pstoredoc, partnerStore, 1, g.goodsId, g.goodsQty, g.goodsPrice, g.goodsTotal, DOC_REASON_INPUT, adraftid, i + 1, "")) {
                        return returnFalse(dw.fErrorMsg, db);
                    }
                }

                int counter = dw.counterAType(DOC_TYPE_CASH);
                QString partnerCashDoc;
                if (!dw.writeAHeader(partnerCashDoc, QString::number(counter), DOC_STATE_DRAFT, DOC_TYPE_CASH, fUser->id(),
                                     QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), fPartnerCode, total,
                                     comment, currencyid)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                if (!dw.writeAHeaderCash(partnerCashDoc, partnerCash, 0, 1, partnerCashDoc, "", 0)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                QString cashUUID;
                if (!dw.writeECash(cashUUID, partnerCashDoc, partnerCash, -1, comment, total, cashUUID, 1)) {
                    return returnFalse(dw.fErrorMsg, db);
                }
                if (!dw.writeAHeaderStore(pstoredoc, fUser->id(), fUser->id(), "", QDate(), partnerStore, 0, 0, partnerCashDoc, 0, 0, fHeader)) {
                    return returnFalse(dw.fErrorMsg, db);
                }

                dw.writeAHeader2ShopStore(pstoredoc, partnerStore, 0);
            }
        }
    }

    db.commit();
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;
        switch (C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;
            if (SelectPrinters::selectPrinters(p1, p2)) {
                if (p1) {
                    p.print(fHeader, db, 1);
                }
                if (p2) {
                    p.print(fHeader, db, 2);
                }
            }
            break;
        }
        case 2:
            p.print2(fHeader, db);
            break;
        case 3:
            p.print3(fHeader, db);
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
    return dw.writeOHeaderFlags(fHeader, f1, f2, f3, f4, f5);
}

bool C5ShopOrder::returnFalse(const QString &msg, C5Database &db)
{
    db.rollback();
    db.close();
    C5LogSystem::writeEvent(msg);
    C5Message::error(msg);
    return false;
}
