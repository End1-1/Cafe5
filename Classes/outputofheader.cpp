#include "outputofheader.h"
#include "oheader.h"
#include "c5storedraftwriter.h"
#include "c5utils.h"

OutputOfHeader::OutputOfHeader(QObject *parent) : QObject(parent)
{
}

bool OutputOfHeader::make(C5Database &db, const QString &id)
{
    C5StoreDraftWriter dw(db);
    db[":f_saleuuid"] = id;
    db.exec("select f_id from a_header_store where f_saleuuid=:f_saleuuid");
    if (db.nextRow() == false) {
        return false;
    }
    QString storeDocId = db.getString(0);
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    OHeader fOHeader;
    if (!fOHeader.getRecord(db)) {
        return false;
    }
    QList<QMap<QString, QVariant> > goodsList;
    db[":f_header"] = fOHeader.id;
    db.exec("select og.f_goods, g.f_name, g.f_unit, og.f_isservice, og.f_qty  "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "where og.f_header=:f_header ");
    while (db.nextRow()) {
        if (db.getInt("f_isservice") == 1) {
            continue;
        }
        QMap<QString, QVariant> m;
        db.rowToMap(m);
        goodsList.append(m);
    }
    db[":f_id"] = fOHeader.partner;
    db.exec("select f_store, f_cash, f_taxname from c_partners where f_id=:f_id");
    QString partnerName;
    if (db.nextRow()) {
        if (db.getInt(0) > 0) {
            double total = 0;
            partnerName = db.getString("f_taxname");
            int partnerStore = db.getInt(0);
            int partnerCash = db.getInt(1);
            QString comment = QString("%1 %2, %3").arg(tr("Store input"), partnerName, fOHeader.humanId());
            QString pstoredoc;
            QString storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, partnerStore, true, 0);
            if (!dw.writeAHeader(pstoredoc, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT, fOHeader.cashier,
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                                 tr("Store input") + " " + partnerName, 0, 1)) {
                return false;
            }
            if (!dw.writeAHeaderStore(pstoredoc, fOHeader.cashier, fOHeader.cashier, "", QDate(), partnerStore, 0, 0, "", 0, 0,
                                      fOHeader._id())) {
                return false;
            }
            // DO NOT UNCOMPLECT THE GOODS WITH UNIT ID THAT EQUAL TO 10
            QString goodsComplect;
            for (const QMap<QString, QVariant> &g : goodsList) {
                if (g["f_unit"].toInt() == 10) {
                    if (!goodsComplect.isEmpty()) {
                        goodsComplect += ",";
                    }
                    goodsComplect += g["f_goods"].toString();
                }
            }
            QList<QMap<QString, QVariant> > ingoods;
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
                    QMap<QString, QVariant> g;
                    g["f_goods"] = db.getInt("f_goods");
                    g["f_qty"] = db.getDouble("f_qty");
                    g["f_price"] = db.getDouble("f_price");
                    g["f_total"] = db.getDouble("f_total");
                    total += db.getDouble("f_total");
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
                QMap<QString, QVariant> g;
                g["f_goods"] = db.getInt("f_goods");
                g["f_qty"] = db.getDouble("f_qty");
                g["f_price"] = db.getDouble("f_price");
                g["f_total"] = db.getDouble("f_total");
                total += db.getDouble("f_total");
                ingoods.append(g);
            }
            for (int i = 0; i < ingoods.count(); i++) {
                const QMap<QString, QVariant> &g = ingoods[i];
                QString adraftid;
                if (!dw.writeAStoreDraft(adraftid, pstoredoc, partnerStore, 1, g["f_goods"].toInt(), g["f_qty"].toDouble(),
                                         g["f_price"].toDouble(), g["f_total"].toDouble(), DOC_REASON_INPUT, adraftid, i + 1, "")) {
                    return false;
                }
            }
            int counter = dw.counterAType(DOC_TYPE_CASH);
            QString partnerCashDoc;
            if (!dw.writeAHeader(partnerCashDoc, QString::number(counter), DOC_STATE_DRAFT, DOC_TYPE_CASH, fOHeader.cashier,
                                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), fOHeader.partner, total,
                                 comment, 0, 1)) {
                return false;
            }
            if (!dw.writeAHeaderCash(partnerCashDoc, partnerCash, 0, 1, partnerCashDoc, "")) {
                return false;
            }
            QString cashUUID;
            if (!dw.writeECash(cashUUID, partnerCashDoc, partnerCash, -1, comment, total, cashUUID, 1)) {
                return false;
            }
            if (!dw.writeAHeaderStore(pstoredoc, fOHeader.cashier, fOHeader.cashier, "", QDate(),
                                      partnerStore, 0, 0, partnerCashDoc, 0, 0, fOHeader._id())) {
                return false;
            }
            dw.writeAHeader2ShopStore(pstoredoc, partnerStore, 0);
        }
    }
    return true;
}
