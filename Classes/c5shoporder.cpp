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

    if (!fOHeader.write(db, err)) {
        return returnFalse(err, db);
    }
    if (!dw.writeOHeaderOptions(fOHeader.id, 0, 0, 0, fOHeader.currency)) {
        return returnFalse(dw.fErrorMsg, db);
    }

    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    if (fOHeader.partner > 0) {
        if (!fBHistory.write(db, err)) {
            return returnFalse(err, db);
        }
    }

    if (C5Config::taxIP().isEmpty()) {
        fOHeader._printFiscal = false;
    }
    if (fOHeader._printFiscal) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), fOHeader.hasIdram() ? "true" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsonIn, jsonOut, err;
        int result = 0;
        pt.fPartnerTin = CPartners().queryRecordOfId(db, fOHeader.id).taxCode;
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
            result = pt.makeJsonAndPrint(fOHeader.amountCard, fOHeader.amountPrepaid, jsonIn, jsonOut, err);
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
            pt.saveTimeResult(fOHeader.id, *q);
            delete q;
        } else {
            pt.saveTimeResult("Not saved - " + fOHeader.id, *q);
            delete q;
            return returnFalse(err + "<br>" + jsonOut + "<br>" + jsonIn, db);
        }
    }

    bool needStoreDoc = false;
    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
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
        if (!dw.writeAHeaderStore(storeDocId, fOHeader.staff, fOHeader.staff, "", QDate(), 0, __c5config.defaultStore(), 1, "", 0, 0, fOHeader.id)) {
            return returnFalse(dw.fErrorMsg, db);
        }
    }

    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.header = fOHeader.id;
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
        b.order = fOHeader.id;
        b.amount = -1 * fOHeader.amountBank;
        b.currency = fOHeader.currency;
        b.write(db, err);
    }
    if (fOHeader.amountDebt > 0.001) {
        BClientDebts b;
        b.source = BCLIENTDEBTS_SOURCE_SALE;
        b.date = fOHeader.dateCash;
        b.costumer = fOHeader.partner;
        b.order = fOHeader.id;
        b.amount = -1 * fOHeader.amountDebt;
        b.currency = fOHeader.currency;
        b.write(db, err);
    }


    /* THIS TO DO FOR ELINA STORE SALE , THAT NEEDED BLOCK TO ITEM CONVERT AND MAKE ORDER INPUT DOCUMENT
     *
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
                                     tr("Store input") + " " + fPartnerName, 0, currencyid)) {
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
                                     comment, 0, currencyid)) {
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
    END OF SECTION. DONT FORGET!
    */

    db.commit();
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;
        switch (C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;
            if (SelectPrinters::selectPrinters(p1, p2)) {
                if (p1) {
                    p.print(fOHeader.id, db, 1);
                }
                if (p2) {
                    p.print(fOHeader.id, db, 2);
                }
            }
            break;
        }
        case 2:
            p.print2(fOHeader.id, db);
            break;
        case 3:
            p.print3(fOHeader.id, db);
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
    return dw.writeOHeaderFlags(fOHeader.id, f1, f2, f3, f4, f5);
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
