#include "c5storedraftwriter.h"
#include "c5utils.h"
#include "c5config.h"
#include <QVariant>
#include <QMap>

C5StoreDraftWriter::C5StoreDraftWriter(C5Database &db) :
    QObject(),
    fDb(db)
{

}

bool C5StoreDraftWriter::writeFromShopOutput(const QString &doc, int state, QString &err)
{
    fDb[":f_id"] = doc;
    fDb.exec("select * from o_header where f_id=:f_id");
    if (!fDb.nextRow()) {
        return returnResult(false);
    }
    QDate docDate = fDb.getDate("f_datecash");
    int operatorId = fDb.getInt("f_staff");
    QString comment = QString("%1 %2%3").arg(tr("Output of sale")).arg(fDb.getString("f_prefix")).arg(fDb.getString("f_hallid"));

    QSet<int> stores;
    QList<IGoods> items;
    fDb[":f_header"] = doc;
    fDb.exec("select f_id, f_store, f_goods, f_qty, f_body, f_tax, f_row, f_discountfactor, f_discountmode from o_goods where f_header=:f_header and f_sign=1");
    while (fDb.nextRow()) {
        IGoods i;
        i.recId = fDb.getString(0);
        i.bodyId = fDb.getString("f_body");
        i.store = fDb.getInt(1);
        i.goodsId = fDb.getInt(2);
        i.goodsQty = fDb.getDouble(3);
        i.goodsPrice = 0;
        i.goodsTotal = 0;
        i.tax = fDb.getInt("f_tax");
        i.row = fDb.getInt("f_row");
        i.discountFactor = fDb.getDouble("f_discountfactor");
        i.discountMode = fDb.getDouble("f_discountmode");
        items.append(i);
        stores.insert(i.store);
    }
    if (items.count() == 0) {
        return true;
    }

    for (int store: stores) {
        int rownum = 1;
        QString id;
        QString userid = storeDocNum(DOC_TYPE_STORE_OUTPUT, store, true, 0);
        writeAHeader(id, userid, state, DOC_TYPE_STORE_OUTPUT, operatorId, docDate, QDate::currentDate(), QTime::currentTime(), 0, 0, comment, 0, 0);
        writeAHeaderStore(id, operatorId, operatorId, "", QDate(), 0, store, 1, "", 0, 0);
        for (IGoods i: items) {
            if (i.store != store) {
                continue;
            }
            QString drid;
            if (!writeAStoreDraft(drid, id, store, -1, i.goodsId, i.goodsQty, i.goodsPrice, i.goodsPrice * i.goodsQty, DOC_REASON_SALE, i.recId, rownum++, "")) {
                err += fDb.fLastError;
                return returnResult(false, err);
            }
            writeOGoods(i.recId, doc, i.bodyId, i.store, i.goodsId, i.goodsQty, i.goodsPrice, i.goodsTotal, i.tax, 1, i.row, drid, i.discountFactor, i.discountMode, 0, 0);
        }
        if (state == DOC_STATE_SAVED) {
            if (!writeOutput(id, err)) {
                haveRelations(id, err, true);
                writeAHeader(id, userid, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, operatorId, docDate, QDate::currentDate(), QTime::currentTime(), 0, 0, comment, 0, 0);
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

QString C5StoreDraftWriter::storeDocNum(int docType, int storeId, bool withUpdate, int value)
{
    QString counterField;
    switch (docType) {
    case DOC_TYPE_STORE_INPUT:
        counterField = "f_inputcounter";
        break;
    case DOC_TYPE_STORE_MOVE:
        counterField = "f_movecounter";
        break;
    case DOC_TYPE_STORE_OUTPUT:
        counterField = "f_outcounter";
        break;
    case DOC_TYPE_COMPLECTATION:
        counterField = "f_complectcounter";
        break;
    }

    fDb[":f_id"] = storeId;
    fDb.exec("select " + counterField + " + 1 from c_storages where f_id=:f_id for update");
    fDb.nextRow();
    int val = fDb.getInt(0);
    if (value > 0) {
        val = value;
    }

    if (withUpdate) {
        fDb[":f_id"] = storeId;
        fDb[":val"] = val;
        fDb.exec("update c_storages set " + counterField + " =:val where f_id=:f_id and " + counterField + "<:val");
    }

    switch (docType) {
    case DOC_TYPE_STORE_INPUT:
        return QString("%1").arg(val, C5Config::docNumDigitsInput(), 10, QChar('0'));
    case DOC_TYPE_STORE_MOVE:
        return QString("%1").arg(val, C5Config::docNumDigitsMove(), 10, QChar('0'));
    case DOC_TYPE_STORE_OUTPUT:
        return QString("%1").arg(val, C5Config::docNumDigitsOut(), 10, QChar('0'));
    case DOC_TYPE_COMPLECTATION:
        return QString("%1").arg(val, C5Config::docNumDigitsInput(), 10, QChar('0'));
    }
    return "ERROR";
}

bool C5StoreDraftWriter::hallId(QString &prefix, int &id, int hall)
{
    fDb[":f_id"] = hall;
    fDb.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    if (fDb.nextRow()) {
        id = fDb.getInt(0);
        prefix = fDb.getString(1);
        fDb[":f_counter"] = fDb.getInt(0);
        fDb.update("h_halls", where_id(hall));
        return true;
    } else {
        fErrorMsg = tr("No hall with id") + "<br>" + QString::number(hall);
        return false;
    }
}

int C5StoreDraftWriter::counterAType(int type)
{
    fDb[":f_id"] = DOC_TYPE_CASH;
    if (!fDb.exec("select f_counter + 1 from a_type where f_id=:f_id for update")) {
        return returnResult(false);
    }
    if (!fDb.nextRow()) {
        return returnResult(false, "Invalid counter number");
    }
    int result = fDb.getInt(0);
    fDb[":f_id"] = type;
    fDb[":f_counter"] = result;
    if (!fDb.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter")) {
       return returnResult(false);
    }
    return result > 0;
}

bool C5StoreDraftWriter::updateField(const QString &tableName, const QString &fieldName, const QVariant &fieldValue, const QString &whereFieldName, const QVariant &whereFieldValue)
{
    fDb[":" + fieldName] = fieldValue;
    fDb[":" + whereFieldName] = whereFieldValue;
    bool r = fDb.exec("update " + tableName + " set " + fieldName + "=:" + fieldName + " where " + whereFieldName + "=:" + whereFieldName);
    if (!r) {
        fErrorMsg = fDb.fLastError;
    }
    return r;
}

bool C5StoreDraftWriter::readAHeaderStore(const QString &id)
{
    fDb[":f_id"] = id;
    if (!fDb.exec("select * from a_header_store where f_id=:f_id", fAHeaderStoreData, fAHeaderStoreDataMap)) {
        fErrorMsg = fDb.fLastError;
        return false;
    }
    if (fAHeaderStoreData.count() > 0) {
        return true;
    } else {
        fErrorMsg = tr("Error in store document, partial data was lost");
        return false;
    }
}

bool C5StoreDraftWriter::readAStoreDraft(const QString &id)
{
    fDb[":f_document"] = id;
    if (fDb.exec("select d.f_id, d.f_goods, "
                 "concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, "
                 "d.f_qty, u.f_name as f_unitname, d.f_price, d.f_total, d.f_reason, d.f_type, d.f_base, d.f_comment "
                 "from a_store_draft d "
                 "left join c_goods g on g.f_id=d.f_goods "
                 "left join c_units u on u.f_id=g.f_unit "
                 "where d.f_document=:f_document order by d.f_row", fAStoreDraftData, fAStoreDraftDataMap)) {
        if (fAStoreDraftData.count() == 0) {
            fErrorMsg = tr("Empty store document");
            return false;
        }
        return true;
    } else {
        fErrorMsg = fDb.fLastError;
        return false;
    }
}

bool C5StoreDraftWriter::readAHeaderCash(const QString &id)
{
    fDb[":f_id"] = id;
    if (!fDb.exec("select * from a_header_cash where f_id=:f_id", fAHeaderCashData, fAHeaderCashDataMap)) {
        fErrorMsg = fDb.fLastError;
        return false;
    }
    if (fAHeaderCashData.count() > 0) {
        return true;
    } else {
        fErrorMsg = tr("Error in cash document, partial data was lost");
        return false;
    }
}

bool C5StoreDraftWriter::readECash(const QString &id)
{
    fDb[":f_header"] = id;
    if (fDb.exec("select * from e_cash where f_header=:f_header order by f_row", fECashData, fECashDataMap)) {
        if (fECashData.count() == 0) {
            fErrorMsg = tr("Empty cash document");
            return false;
        }
        return true;
    } else {
        fErrorMsg = fDb.fLastError;
        return false;
    }
}

bool C5StoreDraftWriter::returnResult(bool r, const QString &msg)
{
    if (!r) {
        if (msg.isEmpty()) {
            fErrorMsg = fDb.fLastError;
        } else {
            fErrorMsg = msg;
        }
    }
    return r;
}

bool C5StoreDraftWriter::writeAHeader(QString &id, const QString &userid, int state, int type, int op, const QDate &docDate, const QDate &dateCreate,
                                     const QTime &timeCreate, int partner, double amount, const QString &comment, int payment, int paid)
{
    bool u = true;
    if (id.isEmpty()) {
        id = fDb.uuid();
        u = false;
    }
    fDb[":f_id"] = id;
    fDb.exec("select f_id from a_header where f_id=:f_id");
    u = fDb.nextRow();
    fDb[":f_id"] = id;
    fDb[":f_userid"] = userid;
    fDb[":f_state"] = state;
    fDb[":f_type"] = type;
    fDb[":f_operator"] = op;
    fDb[":f_date"] = docDate;
    fDb[":f_createdate"] = dateCreate;
    fDb[":f_createtime"] = timeCreate;
    fDb[":f_partner"] = partner;
    fDb[":f_amount"] = amount;
    fDb[":f_comment"] = comment;
    fDb[":f_payment"] = payment;
    fDb[":f_paid"] = paid;
    if (u) {
        return returnResult(fDb.update("a_header", where_id(id)));
    } else {
        return returnResult(fDb.insert("a_header", false));
    }
}

bool C5StoreDraftWriter::readAHeader(const QString &id)
{
    bool result = true;
    fDb[":f_id"] = id;
    fDb.exec("select * from a_header where f_id=:f_id", fAHeaderData, fAHeaderDataMap);
    if (fAHeaderData.count() > 0) {
        switch (value(container_aheader, 0, "f_type").toInt()) {
        case DOC_TYPE_STORE_INPUT:
            result = readAHeaderStore(id);
            if (result) {
                result = readAStoreDraft(id);
            }
            if (value(container_aheader, 0, "f_type").toInt() == DOC_TYPE_STORE_INPUT) {
                if (result) {
                    result = readAHeaderCash(value(container_aheaderstore, 0, "f_cashuuid").toString());
                }
                if (result) {
                    result = readECash(value(container_aheaderstore, 0, "f_cashuuid").toString());
                }
            }
            break;
        case DOC_TYPE_CASH:
            result = readAHeaderCash(id);
            if (result) {
                result = readECash(id);
            }
            break;
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_COMPLECTATION:
            result = readAHeaderStore(id);
            if (result) {
                result = readAStoreDraft(id);
            }
            break;
        }
    } else {
        fErrorMsg = tr("Document not exists");
        return false;
    }
    return result;
}

bool C5StoreDraftWriter::writeAHeaderPartial(QString &id, const QString &userid, int op, const QDate &dateCreate, const QTime &timeCreate, int partner, const QString &comment, int payment, int paid)
{
    fDb[":f_id"] = id;
    fDb[":f_userid"] = userid;
    fDb[":f_operator"] = op;
    fDb[":f_createdate"] = dateCreate;
    fDb[":f_createtime"] = timeCreate;
    fDb[":f_partner"] = partner;
    fDb[":f_comment"] = comment;
    fDb[":f_payment"] = payment;
    fDb[":f_paid"] = paid;
    return returnResult(fDb.update("a_header", where_id(id)));
}

bool C5StoreDraftWriter::writeAHeaderStore(const QString &id, int userAccept, int userPass, const QString &invoice, const QDate &invoiceDate,
                                           int storeIn, int storeOut, int basedOnSale, const QString &cashUUID, int complectation, double complectationQty)
{
    bool u = false;
    fDb[":f_id"] = id;
    fDb.exec("select * from a_header_store where f_id=:f_id");
    u = fDb.nextRow();
    fDb[":f_id"] = id;
    fDb[":f_useraccept"] = userAccept;
    fDb[":f_userpass"] = userPass;
    fDb[":f_invoice"] = invoice;
    fDb[":f_invoicedate"] = invoiceDate;
    fDb[":f_storein"] = storeIn;
    fDb[":f_storeout"] = storeOut;
    fDb[":f_baseonsale"] = basedOnSale;
    fDb[":f_cashuuid"] = cashUUID;
    fDb[":f_complectation"] = complectation;
    fDb[":f_complectationqty"] = complectationQty;
    if (u) {
        return returnResult(fDb.update("a_header_store", where_id(id)));
    } else {
        return returnResult(fDb.insert("a_header_store", false));
    }
}

bool C5StoreDraftWriter::writeAHeaderCash(const QString &id, int cashin, int cashout, int related, const QString &storedoc, const QString &oheader)
{
    bool u = false;
    fDb[":f_id"] = id;
    fDb.exec("select f_id from a_header_cash where f_id=:f_id");
    u = fDb.nextRow();
    fDb[":f_id"] = id;
    fDb[":f_cashin"] = cashin;
    fDb[":f_cashout"] = cashout;
    fDb[":f_related"] = related;
    fDb[":f_storedoc"] = storedoc;
    fDb[":f_oheader"] = oheader;
    if (u) {
        return returnResult(fDb.update("a_header_cash", where_id(id)));
    } else {
        return returnResult(fDb.insert("a_header_cash", false));
    }
}

bool C5StoreDraftWriter::writeAHeader2ShopStore(const QString &id, int store, int accept)
{
    fDb[":f_id"] = id;
    fDb[":f_store"] = store;
    fDb[":f_accept"] = accept;
    return returnResult(fDb.insert("a_header_shop2partner", false));
}

int C5StoreDraftWriter::rowCount(int container)
{
    int rc = -1;
    switch (container) {
    case container_ecash:
        rc = fECashData.count();
        break;
    case container_aheader:
        rc = fAHeaderData.count();
        break;
    case container_aheadercash:
        rc = fAHeaderCashData.count();
        break;
    case container_aheaderstore:
        rc = fAHeaderStoreData.count();
        break;
     case container_astoredraft:
        rc = fAStoreDraftData.count();
        break;
    }
    Q_ASSERT(rc != -1);
    return rc;
}

QVariant C5StoreDraftWriter::value(int container, int row, const QString &key)
{
    QList<QList<QVariant> > *c = nullptr;
    QHash<QString, int> *d = nullptr;
    switch (container) {
    case container_ecash:
        c = &fECashData;
        d = &fECashDataMap;
        break;
    case container_aheader:
        c = &fAHeaderData;
        d = &fAHeaderDataMap;
        break;
    case container_aheadercash:
        c = &fAHeaderCashData;
        d = &fAHeaderCashDataMap;
        break;
    case container_aheaderstore:
        c = &fAHeaderStoreData;
        d = &fAHeaderStoreDataMap;
        break;
    case container_astoredraft:
        c = &fAStoreDraftData;
        d = &fAStoreDraftDataMap;
        break;
    }
    Q_ASSERT(c);
    QList<QVariant> &datarow = (*c)[row];
    int column = d->value(key);
    return datarow.at(column);
}

bool C5StoreDraftWriter::writeAStoreDraft(QString &id, const QString &docId, int store, int type, int goods,
                                          double qty, double price, double total, int reason, const QString &baseid, int rownum,
                                          const QString &comment)
{
    bool u = true;
    if (id.isEmpty()) {
        id = fDb.uuid();
        u = false;
    }
    fDb[":f_id"] = id;
    fDb.exec("select f_id from a_store_draft where f_id=:f_id");
    u = fDb.nextRow();

    fDb[":f_id"] = id;
    fDb[":f_document"] = docId;
    fDb[":f_store"] = store;
    fDb[":f_type"] = type;
    fDb[":f_goods"] = goods;
    fDb[":f_qty"] = qty;
    fDb[":f_price"] = price;
    fDb[":f_total"] = total;
    fDb[":f_reason"] = reason;
    fDb[":f_base"] = baseid;
    fDb[":f_row"] = rownum;
    fDb[":f_comment"] = comment;
    if (u) {
        return returnResult(fDb.update("a_store_draft", where_id(id)));
    } else {
        return returnResult(fDb.insert("a_store_draft", false));
    }
}

bool C5StoreDraftWriter::writeAStoreInventory(QString &id, const QString &docId, int store, int goods, double qty, double price, double total)
{
    fDb[":f_id"] = id;
    fDb.exec("select * from a_store_inventory where f_id=:f_id");
    bool u = fDb.nextRow();
    if (!u) {
        id = fDb.uuid();
    }
    fDb[":f_id"] = id;
    fDb[":f_document"] = docId;
    fDb[":f_store"] = store;
    fDb[":f_goods"] = goods;
    fDb[":f_qty"] = qty;
    fDb[":f_price"] = price;
    fDb[":f_total"] = total;
    if (u) {
        return returnResult(fDb.update("a_store_inventory", "f_id", id));
    } else {
        return returnResult(fDb.insert("a_store_inventory", true));
    }
}

bool C5StoreDraftWriter::writeBHistory(const QString &id, int type, int card, double value, double data)
{
    fDb[":f_id"] = id;
    fDb.exec("select * from b_history");
    bool u = fDb.nextRow();
    fDb[":f_id"] = id;
    fDb[":f_type"] = type;
    fDb[":f_card"] = card;
    fDb[":f_value"] = value;
    fDb[":f_data"] = data;
    if (u) {
        return returnResult(fDb.update("b_history", where_id(id)));
    } else {
        return returnResult(fDb.insert("b_history", false));
    }
}

bool C5StoreDraftWriter::writeECash(QString &id, const QString &header, int cash, int sign, const QString &purpose, double amount, QString &base, int rownum)
{
    bool u = false;
    if (id.isEmpty()) {
        id = fDb.uuid();
    } else {
        u = true;
    }
    if (base.isEmpty()) {
        base = fDb.uuid();
    }
    fDb[":f_id"] = id;
    fDb[":f_header"] = header;
    fDb[":f_cash"] = cash;
    fDb[":f_sign"] = sign;
    fDb[":f_remarks"] = purpose;
    fDb[":f_amount"] = amount;
    fDb[":f_base"] = base;
    fDb[":f_row"] = rownum;
    if (u) {
        return returnResult(fDb.update("e_cash", where_id(id)));
    } else {
        return returnResult(fDb.insert("e_cash", false));
    }
}

bool C5StoreDraftWriter::writeOBody(QString &id, const QString &header, int state, int dish, double qty1, double qty2, double price, double total, double service, double discount,
                                    int store, const QString &print1, const QString &print2, const QString &comment, int remind, const QString &adgcode,
                                    int removereason, int timeorder, int package)
{
    bool u = false;
    if (id.isEmpty()) {
        id = fDb.uuid();
    } else {
        u = true;
    }
    fDb[":f_id"] = id;
    fDb[":f_header"] = header;
    fDb[":f_state"] = state;
    fDb[":f_dish"] = dish;
    fDb[":f_qty1"] = qty1;
    fDb[":f_qty2"] = qty2;
    fDb[":f_price"] = price;
    fDb[":f_total"] = total;
    fDb[":f_service"] = service;
    fDb[":f_discount"] = discount;
    fDb[":f_store"] = store;
    fDb[":f_print1"] = print1;
    fDb[":f_print2"] = print2;
    fDb[":f_comment"] = comment;
    fDb[":f_remind"] = remind;
    fDb[":f_adgcode"] = adgcode;
    fDb[":f_removereason"] = removereason;
    fDb[":f_timeorder"] = timeorder;
    fDb[":f_package"] = package;
    if (u) {
        return returnResult(fDb.update("o_body", where_id(id)));
    } else {
        return returnResult(fDb.insert("o_body", false));
    }
}

bool C5StoreDraftWriter::writeOBodyToOGoods(const QString &id, const QString &headerid)
{
    fDb[":f_body"] = id;
    fDb.exec("delete from o_goods where f_body=:f_body");
    fDb[":f_id"] = id;
    fDb.exec("select r.f_goods, b.f_store, b.f_qty1*r.f_qty as f_qty, g.f_lastinputprice "
             "from d_recipes r "
             "inner join o_body b on b.f_dish=r.f_dish "
             "inner join c_goods g on g.f_id=r.f_goods "
             "where b.f_id=:f_id");
    int row = 0;
    while (fDb.nextRow()) {
        QString gid;
        if (!writeOGoods(gid, headerid, id, fDb.getInt("f_store"), fDb.getInt("f_goods"), fDb.getDouble("f_qty"), fDb.getDouble("f_lastinputprice"),
                         fDb.getDouble("f_qty") * fDb.getDouble("f_lastinputprice"), 0, 1, ++row, "", 0, 0, 0, 0)) {
            return false;
        }
    }
    return true;
}

bool C5StoreDraftWriter::writeOHeader(QString &id, int hallid, const QString &prefix, int state, int hall, int table,
                                      const QDate &dateopen, const QDate &dateclose, const QDate &datecash, const QTime &timeopen, const QTime &timeclose,
                                      int staff, const QString &comment, int print,
                                      double amountTotal, double amountCash, double amountCard, double amountBank, double amountOther,
                                      int serviceMode, double amountService, double amountDiscount, double serviceFactor, double discountFactor,
                                      int creditCardId, int otherId, int shift, int source, int saletype, int partner)
{
    bool u = true;
    if (id.isEmpty()) {
        u = false;
        id = fDb.uuid();
    }
    fDb[":f_id"] = id;
    fDb[":f_hallid"] = hallid;
    fDb[":f_prefix"] = prefix;
    fDb[":f_state"] = state;
    fDb[":f_hall"] = hall;
    fDb[":f_table"] = table;
    fDb[":f_dateopen"] = dateopen;
    fDb[":f_dateclose"] = dateclose;
    fDb[":f_datecash"] = datecash;
    fDb[":f_timeopen"] = timeopen;
    fDb[":f_timeclose"] = timeclose;
    fDb[":f_staff"] = staff;
    fDb[":f_comment"] = comment;
    fDb[":f_print"] = print;
    fDb[":f_amounttotal"] = amountTotal;
    fDb[":f_amountcash"] = amountCash;
    fDb[":f_amountcard"] = amountCard;
    fDb[":f_amountbank"] = amountBank;
    fDb[":f_amountother"] = amountOther;
    fDb[":f_servicemode"] = serviceMode;
    fDb[":f_amountservice"] = amountService;
    fDb[":f_amountdiscount"] = amountDiscount;
    fDb[":f_servicefactor"] = serviceFactor;
    fDb[":f_discountfactor"] = discountFactor;
    fDb[":f_creditcardid"] = creditCardId;
    fDb[":f_otherid"] = otherId;
    fDb[":f_shift"] = shift;
    fDb[":f_source"] = source;
    fDb[":f_saletype"] = saletype;
    fDb[":f_partner"] = partner;
    if (u) {
        return returnResult(fDb.update("o_header", where_id(id)));
    } else {
        return returnResult(fDb.insert("o_header", false));
    }
}

bool C5StoreDraftWriter::writeOGoods(QString &id, const QString &header, const QString &body, int store, int goods,
                                     double qty, double price, double total, int tax, int sign, int row,
                                     const QString &storerec, double discount, int discountMode, int returnMode,
                                     double discFactor)
{
    bool u = true;
    if (id.isEmpty()) {
        id = fDb.uuid();
        u = false;
    }
    fDb[":f_id"] = id;
    fDb[":f_header"] = header;
    fDb[":f_body"] = body;
    fDb[":f_store"] = store;
    fDb[":f_goods"] = goods;
    fDb[":f_qty"] = qty;
    fDb[":f_price"] = price;
    fDb[":f_total"] = total;
    fDb[":f_tax"] = tax;
    fDb[":f_sign"] = sign;
    fDb[":f_row"] = row;
    fDb[":f_storerec"] = storerec;
    fDb[":f_discountfactor"] = discFactor;
    fDb[":f_discountmode"] = discountMode;
    fDb[":f_discountamount"] = discount;
    fDb[":f_return"] = returnMode;
    if (u) {
        return returnResult(fDb.update("o_goods", where_id(id)));
    } else {
        return returnResult(fDb.insert("o_goods", false));
    }
}

bool C5StoreDraftWriter::writeInput(const QString &docId, QString &err)
{
    double total = 0;
    fDb[":f_document"] = docId;
    fDb.exec("select * from a_store_draft where f_document=:f_document");
    QList<QMap<QString, QVariant> > rows;
    while (fDb.nextRow()) {
        rows.append(fDb.getBindValues());
        total += fDb.getDouble("f_total");
    }
    for (int i = 0; i < rows.count(); i++) {
        QMap<QString, QVariant> &r = rows[i];
        r.remove(":f_row");
        r.remove(":f_comment");
        r[":f_basedoc"] = docId;
        r[":f_base"] = r[":f_id"];
        r[":f_draft"] = r[":f_id"];
        fDb.setBindValues(r);
        if (!fDb.insert("a_store", false)) {
            err += fDb.fLastError + "<br>";
            return false;
        }
    }
    updateField("a_header", "f_amount", total, "f_id", docId);
    return true;
}

bool C5StoreDraftWriter::writeOutput(const QString &docId, QString &err)
{
    double amount = 0;
    int storeOut = 0;
    int storeIn = 0;
    int reason = 0;
    int docType = 0;
    QString complectId;
    int complectCode = 0;
    double complectQty = 0;
    QDate date;
    fDb[":f_id"] = docId;
    fDb.exec("select h.f_type, h.f_date, hh.f_storein, hh.f_storeout, hh.f_complectation, hh.f_complectationqty"
               " from a_header h inner join a_header_store hh on hh.f_id=h.f_id where h.f_id=:f_id");
    if (fDb.nextRow()) {
        date = fDb.getDate("f_date");
        storeIn = fDb.getInt("f_storein");
        storeOut = fDb.getInt("f_storeout");
        docType = fDb.getInt("f_type");
        complectCode = fDb.getInt("f_complectation");
        complectQty = fDb.getDouble("f_complectationqty");
    } else {
        err += tr("Invalid document id") + "<br>";
    }
    if (complectCode > 0) {
        fDb[":f_document"] = docId;
        fDb[":f_type"] = 1;
        fDb.exec("select f_id from a_store_draft where f_document=:f_document and f_type=:f_type");
        if (fDb.nextRow()) {
            complectId = fDb.getString(0);
        }
    }

    QStringList recID;
    QStringList baseID;
    QStringList goodsID;
    QStringList goodsName;
    QList<double> qtyList;
    QList<double> priceList;
    QList<double> totalList;
    fDb[":f_document"] = docId;
    fDb.exec("select s.f_id, s.f_goods, s.f_store, s.f_qty, s.f_price, s.f_total, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, s.f_base, s.f_reason "
               "from a_store_draft s inner join c_goods g on g.f_id=s.f_goods "
               "where f_document=:f_document and f_type=-1");
    while (fDb.nextRow()) {
        recID.append(fDb.getString("f_id"));
        baseID.append(fDb.getString("f_base"));
        goodsID.append(fDb.getString("f_goods"));
        goodsName.append(fDb.getString("f_name"));
        reason = fDb.getInt("f_reason");
        qtyList.append(fDb.getDouble("f_qty"));
        priceList.append(fDb.getDouble("f_price"));
        totalList.append(fDb.getDouble("f_total"));
    }

    QList<QList<QVariant> > storeData;
    fDb[":f_store"] = storeOut;
    fDb[":f_date"] = date;
    fDb[":f_state"] = DOC_STATE_SAVED;
    if (!fDb.exec(QString("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type), s.f_price, sum(s.f_total*s.f_type) "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date and d.f_state=:f_state "
            "group by 1, 2, 4 "
            "having sum(s.f_qty*s.f_type) > 0.00001 "
            "for update ").arg(goodsID.join(",")), storeData)) {
        err = fDb.fLastError + "<br>";
        return false;
    }
    QList<QMap<QString, QVariant> > queries;
    for (int i = 0; i < goodsID.count(); i++) {
        double qty = qtyList.at(i);
        totalList[i] = 0;
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j).at(1).toInt() == goodsID.at(i).toInt()) {
                if (storeData.at(j).at(2).toDouble() > 0) {
                    if (storeData.at(j).at(2).toDouble() >= qty) {
                        storeData[j][2] = storeData.at(j).at(2).toDouble() - qty;
                        fDb[":f_base"] = storeData.at(j).at(0).toString();
                        fDb.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        fDb.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = fDb.getString(0);
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        amount += storeData.at(j).at(3).toDouble() * qty;
                        totalList[i] = totalList[i] + (storeData.at(j).at(3).toDouble() * qty);
                        priceList[i] = totalList[i] / qtyList.at(i);
                        qty = 0;
                    } else {
                        fDb[":f_base"] = storeData.at(j).at(0).toString();
                        fDb.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        fDb.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = storeData.at(j).at(2).toDouble();
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = fDb.getString(0);
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        totalList[i] = totalList[i] + (storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble());
                        priceList[i] = totalList[i] / qtyList.at(i);
                        amount += storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        qty -= storeData.at(j).at(2).toDouble();
                        storeData[j][2] = 0.0;
                    }
                }
            }
            if (qty < 0.001) {
                break;
            }
        }
        if (qty > 0.00001) {
            if (err.isEmpty()) {
                err += tr("Not enough materials in the store") + "<br>";
            }
            err += QString("%1 - %2").arg(goodsName.at(i)).arg(qty) + "<br>";
        }
    }
    if (err.isEmpty()) {
        QStringList outId;
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.begin(); it != queries.end(); it++) {
            QString newId = C5Database::uuid();
            outId << newId;
            fDb.setBindValues(*it);
            fDb[":f_id"] = newId;
            fDb.insert("a_store", false);
        }
        writeTotalStoreAmount(docId);
        if (storeIn > 0) {
            switch (docType) {
            case DOC_TYPE_STORE_MOVE:
                foreach (QString recid, outId) {
                    fDb[":f_id"] = recid;
                    fDb.exec("select * from a_store where f_id=:f_id");
                    fDb.nextRow();
                    fDb.setBindValues(fDb.getBindValues());
                    fDb.removeBindValue(":f_id");
                    fDb[":f_id"] = C5Database::uuid();
                    fDb[":f_document"] = docId;
                    fDb[":f_type"] = 1;
                    fDb[":f_store"] = storeIn;
                    fDb.insert("a_store", false);
                }
                break;
            case DOC_TYPE_COMPLECTATION:
                fDb[":f_header"] = docId;
                fDb.exec("select sum(f_amount) from a_complectation_additions where f_header=:f_header");
                fDb.nextRow();
                amount += fDb.getDouble(0);
                QString id = C5Database::uuid();
                fDb[":f_id"] = id;
                fDb[":f_document"] = docId;
                fDb[":f_store"] = storeIn;
                fDb[":f_type"] = 1;
                fDb[":f_goods"] = complectCode;
                fDb[":f_qty"] = complectQty;
                fDb[":f_price"] = amount / complectQty;
                fDb[":f_total"] = amount;
                fDb[":f_base"] = outId.at(0);
                fDb[":f_basedoc"] = docId;
                fDb[":f_reason"] = reason;
                fDb[":f_draft"] = complectId;
                fDb.insert("a_store", false);
                break;
            }
        }
        for (int i = 0; i < recID.count(); i++) {
            fDb[":f_price"] = priceList.at(i);
            fDb[":f_total"] = totalList.at(i);
            fDb.update("a_store_draft", where_id(recID.at(i)));
            if (!baseID.at(i).isEmpty()) {
                fDb[":f_price"] = priceList.at(i);
                fDb[":f_total"] = totalList.at(i);
                fDb.update("a_store_draft", where_id(baseID.at(i)));
            }
        }
        if (docType == DOC_TYPE_COMPLECTATION) {
            fDb[":f_type"] = 1;
            fDb[":f_document"] = docId;
            fDb[":f_price"] = amount / complectQty;
            fDb[":f_total"] = amount;
            fDb.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_type=:f_type and f_document=:f_document");
        }
    }
    return err.isEmpty();
}

bool C5StoreDraftWriter::writeTotalStoreAmount(const QString &docId)
{
    fDb[":f_document"] = docId;
    fDb.exec("select f_type, sum(f_total) as f_total from a_store where f_document=:f_document group by 1");
    if (fDb.nextRow()) {
        fDb[":f_amount"] = fDb.getDouble("f_total");
        fDb.update("a_header", where_id(docId));
    }
    return true;
}

bool C5StoreDraftWriter::haveRelations(const QString &id, QString &err, bool clear)
{
    fDb[":f_basedoc"] = id;
    fDb.exec("select distinct(s.f_document) as f_document, d.f_date, d.f_userid from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
    err = "";
    while (fDb.nextRow()) {
        err += QString("No: %1, %2<br>").arg(fDb.getString("f_userid")).arg(fDb.getDate("f_date").toString(FORMAT_DATE_TO_STR));
    }
    if (!err.isEmpty()) {
        err = tr("This order used in next documents") + "<br>" + err;
    }
    if (clear && err.isEmpty()) {
        fDb[":f_document"] = id;
        fDb.exec("delete from a_store where f_document=:f_document");
    }
    return !err.isEmpty();
}

bool C5StoreDraftWriter::clearAStoreDraft(const QString &id)
{
    if (id.isEmpty()) {
        return true;
    }
    fDb[":f_document"] = id;
    fDb.exec("delete from a_store_draft where f_document=:f_document");
    return true;
}
