#include "c5storedraftwriter.h"
#include "c5utils.h"
#include "c5config.h"
#include <QJsonDocument>
#include <QJsonObject>

struct Item {
    QString id;
    int store;
    int goods;
    double qty;
    bool changed;
    Item() {changed = false; }
};

C5StoreDraftWriter::C5StoreDraftWriter(C5Database &db) :
    QObject(),
    fDb(db)
{

}

bool C5StoreDraftWriter::writeFromShopInput(const QDate &date, const QString &doc)
{
    QSet<int> stores;
    QList<Item> items;
    fDb[":f_header"] = doc;
    fDb.exec("select f_id, f_store, f_goods, f_qty from o_goods where f_header=:f_header");
    while (fDb.nextRow()) {
        Item i;
        i.id = fDb.getString(0);
        i.store = fDb.getInt(1);
        i.goods = fDb.getInt(2);
        i.qty = fDb.getDouble(3);
        items.append(i);
        stores.insert(i.store);
    }
    if (items.count() == 0) {
        return true;
    }

    for (int store: stores) {
        QString docid;
        fDb[":f_state"] = DOC_STATE_DRAFT;
        fDb[":f_type"] = DOC_TYPE_STORE_INPUT;
        fDb[":f_reason"] = DOC_REASON_SALE_RETURN;
        fDb[":f_date"] = date;
        fDb[":f_store"] = store;
        fDb.exec("select h.f_id from a_header h "
                 "inner join a_store_draft d on d.f_document=h.f_id "
                 "where h.f_date=:f_date and h.f_type=:f_type and h.f_state=:f_state "
                 "and d.f_store=:f_store and d.f_reason=:f_reason");
        if (fDb.nextRow()) {
            docid = fDb.getString(0);
        } else {
            docid = C5Database::uuid();
            QJsonObject jo;
            jo["f_storein"] = QString::number(store);
            jo["f_storeout"] = "";
            jo["based_on_sale"] = 1;
            QJsonDocument jd(jo);
            fDb[":f_userid"] = "";
            fDb[":f_state"] = DOC_STATE_DRAFT;
            fDb[":f_type"] = DOC_TYPE_STORE_INPUT;
            fDb[":f_operator"] = __userid;
            fDb[":f_date"] = date;
            fDb[":f_createDate"] = QDate::currentDate();
            fDb[":f_createTime"] = QTime::currentTime();
            fDb[":f_partner"] = 0;
            fDb[":f_amount"] = 0;
            fDb[":f_comment"] = tr("Sale autoinput");
            fDb[":f_invoice"] = "";
            fDb[":f_raw"] = jd.toJson(QJsonDocument::Compact);
            fDb[":f_payment"] = 0;
            fDb[":f_paid"] = 0;
            fDb[":f_id"] = docid;
            if (fDb.insert("a_header", false) == 0) {
                fErrorMsg = fDb.fLastError;
                return false;
            }
        }

        QMap<int, Item> draft;
        fDb[":f_document"] = docid;
        fDb.exec("select f_id, f_goods, f_qty from a_store_draft where f_document=:f_document");
        while (fDb.nextRow()) {
            Item i;
            i.id = fDb.getString(0);
            i.goods = fDb.getInt(1);
            i.qty = fDb.getDouble(2);
            draft.insert(i.goods, i);
        }

        for (Item i: items) {
            if (i.store != store) {
                continue;
            }
            if (!draft.contains(i.goods)) {
                QString draftId = C5Database::uuid();
                fDb[":f_id"] = draftId;
                fDb[":f_document"] = docid;
                fDb[":f_store"] = store;
                fDb[":f_type"] = 1;
                fDb[":f_goods"] = i.goods;
                fDb[":f_qty"] = 0;
                fDb[":f_price"] = 0;
                fDb[":f_total"] = 0;
                fDb[":f_reason"] = DOC_REASON_SALE_RETURN;
                fDb.insert("a_store_draft", false);
                Item ii;
                ii.id = draftId;
                ii.goods = i.goods;
                ii.qty = 0;
                ii.store = store;
                draft.insert(ii.goods, ii);
            }
            Item &storeItem = draft[i.goods];
            storeItem.qty += i.qty;
            storeItem.changed = true;
            fDb[":f_storerec"] = storeItem.id;
            fDb[":f_id"] = i.id;
            fDb.exec("update o_goods set f_storerec=:f_storerec where f_id=:f_id");
        }

        for (Item i: draft) {
            if (i.changed) {
                fDb[":f_qty"] = i.qty;
                fDb[":f_id"] = i.id;
                fDb.exec("update a_store_draft set f_qty=:f_qty where f_id=:f_id");
            }
        }
    }
    return true;
}

bool C5StoreDraftWriter::writeFromShopOutput(const QDate &date, const QString &doc)
{
    QSet<int> stores;
    QList<Item> items;
    fDb[":f_header"] = doc;
    fDb.exec("select f_id, f_store, f_goods, f_qty from o_goods where f_header=:f_header");
    while (fDb.nextRow()) {
        Item i;
        i.id = fDb.getString(0);
        i.store = fDb.getInt(1);
        i.goods = fDb.getInt(2);
        i.qty = fDb.getDouble(3);
        items.append(i);
        stores.insert(i.store);
    }
    if (items.count() == 0) {
        return true;
    }

    for (int store: stores) {
        QString docid;
        fDb[":f_state"] = DOC_STATE_DRAFT;
        fDb[":f_type"] = DOC_TYPE_STORE_OUTPUT;
        fDb[":f_reason"] = DOC_REASON_SALE;
        fDb[":f_date"] = date;
        fDb[":f_store"] = store;
        fDb.exec("select h.f_id from a_header h "
                 "inner join a_store_draft d on d.f_document=h.f_id "
                 "where h.f_date=:f_date and h.f_type=:f_type and h.f_state=:f_state "
                 "and d.f_store=:f_store and d.f_reason=:f_reason");
        if (fDb.nextRow()) {
            docid = fDb.getString(0);
        } else {
            docid = C5Database::uuid();
            QJsonObject jo;
            jo["f_storein"] = "";
            jo["f_storeout"] = QString::number(store);
            jo["based_on_sale"] = 1;
            QJsonDocument jd(jo);
            fDb[":f_userid"] = "";
            fDb[":f_state"] = DOC_STATE_DRAFT;
            fDb[":f_type"] = DOC_TYPE_STORE_OUTPUT;
            fDb[":f_operator"] = __userid;
            fDb[":f_date"] = date;
            fDb[":f_createDate"] = QDate::currentDate();
            fDb[":f_createTime"] = QTime::currentTime();
            fDb[":f_partner"] = 0;
            fDb[":f_amount"] = 0;
            fDb[":f_comment"] = tr("Sale autooutput");
            fDb[":f_invoice"] = "";
            fDb[":f_raw"] = jd.toJson(QJsonDocument::Compact);
            fDb[":f_payment"] = 0;
            fDb[":f_paid"] = 0;
            fDb[":f_id"] = docid;
            if (fDb.insert("a_header", false) == 0) {
                fErrorMsg = fDb.fLastError;
                return false;
            }
        }

        QMap<int, Item> draft;
        fDb[":f_document"] = docid;
        fDb.exec("select f_id, f_goods, f_qty from a_store_draft where f_document=:f_document");
        while (fDb.nextRow()) {
            Item i;
            i.id = fDb.getString(0);
            i.goods = fDb.getInt(1);
            i.qty = fDb.getDouble(2);
            draft.insert(i.goods, i);
        }

        for (Item i: items) {
            if (i.store != store) {
                continue;
            }
            if (!draft.contains(i.goods)) {
                QString draftId = C5Database::uuid();
                fDb[":f_id"] = draftId;
                fDb[":f_document"] = docid;
                fDb[":f_store"] = store;
                fDb[":f_type"] = -1;
                fDb[":f_goods"] = i.goods;
                fDb[":f_qty"] = 0;
                fDb[":f_price"] = 0;
                fDb[":f_total"] = 0;
                fDb[":f_reason"] = DOC_REASON_SALE;
                fDb.insert("a_store_draft", false);
                Item ii;
                ii.id = draftId;
                ii.goods = i.goods;
                ii.qty = 0;
                ii.store = store;
                draft.insert(ii.goods, ii);
            }
            Item &storeItem = draft[i.goods];
            storeItem.qty += i.qty;
            storeItem.changed = true;
            fDb[":f_storerec"] = storeItem.id;
            fDb[":f_id"] = i.id;
            fDb.exec("update o_goods set f_storerec=:f_storerec where f_id=:f_id");
        }

        for (Item i: draft) {
            if (i.changed) {
                fDb[":f_qty"] = i.qty;
                fDb[":f_id"] = i.id;
                fDb.exec("update a_store_draft set f_qty=:f_qty where f_id=:f_id");
            }
        }
    }
    return true;
}

bool C5StoreDraftWriter::rollbackOutput(C5Database &db, const QString &id)
{
    db[":f_id"] = id;
    db.exec("select f_storerec, f_qty from o_goods where f_id=:f_id");
    if (db.nextRow()) {
        QString recid = db.getString("f_storerec");
        double qty = db.getDouble("f_qty");
        db[":f_id"] = recid;
        db.exec("select f_document, f_qty from a_store_draft where f_id=:f_id for update");
        if (db.nextRow()) {
            double draftQty = db.getDouble("f_qty");
            db[":f_qty"] = qty;
            db[":f_id"] = recid;
            db.exec("update a_store_draft set f_qty=f_qty-:f_qty where f_id=:f_id");
            if (draftQty - qty < 0.0001) {
                db[":f_id"] = recid;
                db.exec("delete from a_store_draft where f_id=:f_id");
            }
            db[":f_id"] = db.getString("f_document");
            db.exec("select * from a_header where f_id=:f_id");
            if (db.nextRow()) {
                if (db.getInt("f_state") == DOC_STATE_SAVED) {
                    fErrorMsg = tr("Store document has been registered for this sale");
                    return false;
                }
            }
        }
    }
    db[":f_id"] = id;
    db.exec("delete from o_goods where f_id=:f_id");
    return true;
}

QString C5StoreDraftWriter::writeDraft(const QDate &date, int doctype, int store, int reason, const QMap<int, double> &data, const QString &comment)
{
    QJsonObject jo;
    int type = 0;
    switch (doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        jo["f_storein"] = "";
        jo["f_storeout"] = QString::number(store);
        type = -1;
        break;
    case DOC_TYPE_STORE_INPUT:
        jo["f_storein"] = QString::number(store);
        jo["f_storeout"] = "";
        type = 1;
        break;
    }
    QJsonDocument jd(jo);
    QString fDocumentId;
    fDb[":f_state"] = DOC_STATE_DRAFT;
    fDb[":f_type"] = doctype;
    fDb[":f_operator"] = __userid;
    fDb[":f_date"] = date;
    fDb[":f_createDate"] = QDate::currentDate();
    fDb[":f_createTime"] = QTime::currentTime();
    fDb[":f_partner"] = 0;
    fDb[":f_amount"] = 0;
    fDb[":f_comment"] = comment;
    fDb[":f_raw"] = jd.toJson();
    fDb[":f_id"]= fDocumentId;
    fDb[":f_payment"] = 0;
    fDb[":f_paid"] = 0;
    if (fDocumentId.isEmpty()) {
        fDocumentId = C5Database::uuid();
        fDb[":f_id"] = fDocumentId;
        fDb.insert("a_header", false);
    } else {
        fDb.update("a_header", where_id(fDocumentId));
        fDb[":f_document"] = fDocumentId;
        fDb.exec("delete from a_store_draft where f_document=:f_document");
    }
    for (QMap<int, double>::const_iterator it = data.begin(); it != data.end(); it++) {
        fDb[":f_id"] = C5Database::uuid();
        fDb[":f_document"] = fDocumentId;
        fDb[":f_type"] = type;
        fDb[":f_goods"] = it.key();
        fDb[":f_qty"] = it.value();
        fDb[":f_price"] = 0;
        fDb[":f_total"] = 0;
        fDb[":f_reason"] = reason;
        fDb[":f_store"] = store;
        fDb.insert("a_store_draft", false);
    }
    return fDocumentId;
}