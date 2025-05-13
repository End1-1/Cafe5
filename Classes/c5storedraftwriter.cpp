#include "c5storedraftwriter.h"
#include "c5utils.h"
#include "c5config.h"
#include "chatmessage.h"
#include "c5cafecommon.h"
#include "jsons.h"
#include "ogoods.h"
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

struct tmpg {
    QString recId;
    int goodsId;
    double qtyDraft;
    double qtyGoods;
    int store;
    double price;
    tmpg() {}
    tmpg(const QString &n, double dr, double goods, double pr)
    {
        recId = n;
        qtyDraft = dr;
        qtyGoods = goods;
        price = pr;
    }
};

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
    QString comment = QString("%1 %2%3").arg(tr("Output of sale"), fDb.getString("f_prefix"),
                      QString::number(fDb.getInt("f_hallid")));
    QSet<int> stores;
    QList<OGoods> items;
    fDb[":f_header"] = doc;
    fDb.exec("select * from o_goods where f_header=:f_header and f_sign=1");
    bool fetch = false;
    do {
        OGoods g;
        fetch = g.getRecord(fDb);
        if (fetch) {
            items.append(g);
            stores.insert(g.store);
        }
    } while (fetch);
    if (items.count() == 0) {
        return true;
    }
    for (int store : stores) {
        int rownum = 1;
        QString id;
        QString userid = storeDocNum(DOC_TYPE_STORE_OUTPUT, store, true, 0);
        writeAHeader(id, userid, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, operatorId, docDate,
                     QDate::currentDate(), QTime::currentTime(), 0, 0, comment,
                     0, __c5config.getValue(param_default_currency).toInt());
        writeAHeaderStore(id, operatorId, operatorId, "", QDate(), 0, store, 1, "", 0, 0, doc);
        for (OGoods i : items) {
            if (i.store != store) {
                continue;
            }
            QString drid;
            if (!writeAStoreDraft(drid, id, store, -1, i.goods, i.qty, i.price, i.price *i.qty, DOC_REASON_SALE, i.id.toString(),
                                  rownum++, "")) {
                err += fDb.fLastError;
                return returnResult(false, err);
            }
            i.storeRec = drid;
            if (!i.write(fDb, err)) {
                return returnResult(false, err);
            }
        }
        if (state == DOC_STATE_SAVED) {
            if (writeOutput(id, err)) {
                writeAHeader(id, userid, DOC_STATE_SAVED, DOC_TYPE_STORE_OUTPUT, operatorId, docDate, QDate::currentDate(),
                             QTime::currentTime(), 0, 0, comment, 0, __c5config.getValue(param_default_currency).toInt());
            } else {
                haveRelations(id, err, true);
                writeAHeader(id, userid, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, operatorId, docDate, QDate::currentDate(),
                             QTime::currentTime(), 0, 0, comment, 0, __c5config.getValue(param_default_currency).toInt());
            }
        }
    }
    return true;
}

bool C5StoreDraftWriter::rollbackOutput(C5Database &db, const QString &id)
{
    QSet<QString> docs;
    db[":f_id"] = id;
    db.exec("select d.f_document, g.f_storerec "
            "from o_goods g "
            "inner join a_store_draft d on g.f_storerec=d.f_id "
            "where g.f_id=:f_id");
    while (db.nextRow()) {
        docs.insert(db.getString(0));
    }
    db[":f_id"] = id;
    db.exec("delete from o_goods where f_id=:f_id");
    for (const QString &doc : docs) {
        QString err;
        if (!removeStoreDocument(db, doc, err)) {
            return false;
        }
    }
    return true;
}

bool C5StoreDraftWriter::removeStoreDocument(C5Database &db, const QString &id, QString &err)
{
    C5StoreDraftWriter dw(db);
    if (dw.haveRelations(id, err, false)) {
        return false;
    }
    db[":f_id"] = id;
    db.exec("select f_baseonsale, f_cashuuid from a_header_store where f_id=:f_id");
    QString cashDoc;
    if (db.nextRow()) {
        if (db.getInt(0) > 0) {
            //err += tr("Document based on sale cannot be edited manually");
        }
        cashDoc = db.getString("f_cashuuid");
    }
    if (err.isEmpty()) {
        if (!dw.outputRollback(db, id)) {
            err += db.fLastError;
        }
    }
    if (!err.isEmpty()) {
        return false;
    }
    int storei = 0;
    int storeo = 0;
    QStringList sqls;
    db[":f_id"] = id;
    db.exec("select f_storein, f_storeout from a_header_store where f_id=:f_id");
    if (db.nextRow()) {
        storei = db.getInt(0);
        storeo = db.getInt(1);
    }
    db[":f_id1"] = storei;
    db[":f_id2"] = storeo;
    sqls.append(db.execDry("update c_storages set f_have_changes=1 where f_id=:f_id1 or f_id=:f_id2"));
    db[":f_document"] = id;
    sqls.append(db.execDry("delete from a_store_draft where f_document=:f_document"));
    db[":f_storedoc"] = id;
    sqls.append(db.execDry("delete from b_clients_debts where f_storedoc=:f_storedoc"));
    db[":f_id"] = id;
    sqls.append(db.execDry("delete from a_header where f_id=:f_id"));
    db[":f_id"] = id;
    sqls.append(db.execDry("delete from a_header_store where f_id=:f_id"));
    if (!cashDoc.isEmpty()) {
        db[":f_header"] = cashDoc;
        sqls.append(db.execDry("delete from e_cash where f_header=:f_header"));
        db[":f_id"] = cashDoc;
        sqls.append(db.execDry("delete from a_header_cash where f_id=:f_id"));
        db[":f_id"] = cashDoc;
        sqls.append(db.execDry("delete from a_header where f_id=:f_id"));
    }
    db[":f_document"] = id;
    sqls.append(db.execDry("delete from a_store_dish_waste where f_document=:f_document"));
    db.execSqlList(sqls);
    return err.isEmpty();
}

bool C5StoreDraftWriter::outputRollback(C5Database &db, const QString &id)
{
    db[":f_document"] = id;
    if (!db.exec("delete from a_store where f_document=:f_document")) {
        return false;
    }
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
    if (!fDb.nextRow()) {
        return "UNKNOWN STORE";
    }
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
    if (!fDb.exec("select cast(f_counter + 1 as int) "
                  "from a_type "
                  "where f_id=:f_id")) {
        return returnResult(false);
    }
    if (!fDb.nextRow()) {
        return returnResult(false, "Invalid counter number");
    }
    int result = fDb.getInt(0);
    Q_ASSERT(result > 0);
    fDb[":f_id"] = type;
    fDb[":f_counter"] = result;
    if (!fDb.exec("update a_type set f_counter=:f_counter "
                  "where f_id=:f_id and f_counter<:f_counter")) {
        return returnResult(false);
    }
    return result > 0;
}

bool C5StoreDraftWriter::updateField(const QString &tableName, const QString &fieldName, const QVariant &fieldValue,
                                     const QString &whereFieldName, const QVariant &whereFieldValue)
{
    fDb[":" + fieldName] = fieldValue;
    fDb[":" + whereFieldName] = whereFieldValue;
    bool r = fDb.exec("update " + tableName + " set " + fieldName + "=:" + fieldName + " where " + whereFieldName + "=:" +
                      whereFieldName);
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
    if (fAHeaderStoreData.size() > 0) {
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
                 "d.f_qty, u.f_name as f_unitname, d.f_price, d.f_total, d.f_reason, d.f_type, d.f_base, d.f_comment, "
                 "v.f_date as f_validdate "
                 "from a_store_draft d "
                 "left join c_goods g on g.f_id=d.f_goods "
                 "left join c_units u on u.f_id=g.f_unit "
                 "left join a_store_valid v on v.f_id=d.f_id "
                 "where d.f_document=:f_document order by d.f_row", fAStoreDraftData, fAStoreDraftDataMap)) {
        //        if (fAStoreDraftData.count() == 0) {
        //            fErrorMsg = tr("Empty store document");
        //            return false;
        //        }
        return true;
    } else {
        fErrorMsg = fDb.fLastError;
        return false;
    }
}

bool C5StoreDraftWriter::readAStoreDishWaste(const QString &id)
{
    fDb[":f_document"] = id;
    if (!fDb.exec("select w.f_id, w.f_dish, d.f_name as f_dishname, w.f_qty, w.f_data "
                  "from a_store_dish_waste w "
                  "left join d_dish d on d.f_id=w.f_dish "
                  "where w.f_document=:f_document", fAStoreDishWaste, fAStoreDishWasteDataMap)) {
        fErrorMsg = fDb.fLastError;
        return false;
    }
    return true;
}

bool C5StoreDraftWriter::readAHeaderCash(const QString &id)
{
    fDb[":f_id"] = id;
    if (!fDb.exec("select * from a_header_cash where f_id=:f_id", fAHeaderCashData, fAHeaderCashDataMap)) {
        fErrorMsg = fDb.fLastError;
        return false;
    }
    if (fAHeaderCashData.size() > 0) {
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
        if (fECashData.size() == 0) {
            fErrorMsg = tr("Empty cash document");
            return false;
        }
        return true;
    } else {
        fErrorMsg = fDb.fLastError;
        return false;
    }
}

bool C5StoreDraftWriter::readBClientDebtsRefund(const QString &id)
{
    fDb[":f_storedoc"] = id;
    if (fDb.exec("select * from b_clients_debts where f_storedoc=:f_storedoc", fBClientsDebtsData, fBClientsDebtsDataMap)) {
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

bool C5StoreDraftWriter::writeAHeader(QString &id, const QString &userid, int state, int type, int op,
                                      const QDate &docDate, const QDate &dateCreate,
                                      const QTime &timeCreate, int partner, double amount,
                                      const QString &comment, int paid, int currency,
                                      int working_session)
{
    bool u = true;
    if (id.isEmpty()) {
        id = fDb.uuid();
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
    fDb[":f_paid"] = paid;
    fDb[":f_working_session"] = working_session;
    fDb[":f_currency"] = currency;
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
    if (fAHeaderData.size() > 0) {
        switch (value(container_aheader, 0, "f_type").toInt()) {
            case DOC_TYPE_STORE_INPUT:
                result = readAHeaderStore(id);
                if (result) {
                    result = readAStoreDraft(id);
                }
                if (result) {
                    result = readBClientDebtsRefund(id);
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
        if (result) {
            result = readAStoreDishWaste(id);
        }
    } else {
        fErrorMsg = tr("Document not exists");
        return false;
    }
    return result;
}

bool C5StoreDraftWriter::writeAHeaderPartial(QString &id, const QString &userid, int op, const QDate &dateCreate,
        const QTime &timeCreate, int partner, const QString &comment)
{
    fDb[":f_id"] = id;
    fDb[":f_userid"] = userid;
    fDb[":f_operator"] = op;
    fDb[":f_createdate"] = dateCreate;
    fDb[":f_createtime"] = timeCreate;
    fDb[":f_partner"] = partner;
    fDb[":f_comment"] = comment;
    return returnResult(fDb.update("a_header", where_id(id)));
}

bool C5StoreDraftWriter::writeAHeaderStore(const QString &id, int userAccept, int userPass, const QString &invoice,
        const QDate &invoiceDate,
        int storeIn, int storeOut, int basedOnSale, const QString &cashUUID, int complectation,
        double complectationQty, const QString &saleuuid)
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
    fDb[":f_saleuuid"] = saleuuid;
    if (u) {
        return returnResult(fDb.update("a_header_store", where_id(id)));
    } else {
        return returnResult(fDb.insert("a_header_store", false));
    }
}

bool C5StoreDraftWriter::writeAHeaderCash(const QString &id,
        int cashin, int cashout,
        int related,
        const QString &storedoc,
        const QString &oheader)
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
    unsigned long long rc = -1;
    switch (container) {
        case container_ecash:
            rc =  fECashData.size();
            break;
        case container_aheader:
            rc = fAHeaderData.size();
            break;
        case container_aheadercash:
            rc = fAHeaderCashData.size();
            break;
        case container_aheaderstore:
            rc = fAHeaderStoreData.size();
            break;
        case container_astoredraft:
            rc = fAStoreDraftData.size();
            break;
        case container_astoredishwaste:
            rc = fAStoreDishWaste.size();
            break;
        case container_bclient_debts:
            rc = fBClientsDebtsData.size();
            break;
    }
    Q_ASSERT(rc != -1);
    return rc;
}

QVariant C5StoreDraftWriter::value(int container, int row, const QString &key)
{
    std::vector<QJsonArray > *c = nullptr;
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
        case container_astoredishwaste:
            c = &fAStoreDishWaste;
            d = &fAStoreDishWasteDataMap;
            break;
        case container_bclient_debts:
            c = &fBClientsDebtsData;
            d = &fBClientsDebtsDataMap;
            break;
    }
    Q_ASSERT(c);
    QJsonArray  &datarow = ( *c)[row];
    int column = d->value(key);
    return datarow.at(column);
}

bool C5StoreDraftWriter::writeStoreOfSale(const QString &uuid,  QString &err, int storedocstate)
{
    //Remove old docs
    C5Database db(__c5config.dbParams());
    db[":f_id"] = uuid;
    db.exec("select distinct(ad.f_document) from o_goods g "
            "left join a_store_draft ad on ad.f_id=g.f_storerec "
            "where g.f_header=:f_id");
    while (db.nextRow()) {
        removeStoreDocument(fDb, db.getString(0), err);
    }
    //Check for store doc
    db[":f_header"] = uuid;;
    db.exec("select h.f_state, d.f_id, d.f_qty as f_dqty, g.f_qty as f_gqty, "
            "gg.f_lastinputprice as f_gprice "
            "from o_goods g "
            "inner join a_store_draft d on d.f_id=g.f_storerec "
            "inner join a_header h on h.f_id=d.f_document "
            "inner join c_goods gg on g.f_id=g.f_goods "
            "where g.f_header=:f_header");
    QList<tmpg> goods;
    while (db.nextRow()) {
        if (db.getInt("f_state") == DOC_STATE_SAVED) {
            err += tr("Document saved") + "<br>";
            return false;
        }
        goods.append(tmpg(db.getString("f_id"), db.getDouble("f_dqty"), db.getDouble("f_gqty"), db.getDouble("f_gprice")));
    }
    foreach (const tmpg &t, goods) {
        db[":f_id"] = t.recId;
        db[":f_qty"] = t.qtyGoods;
        db.exec("update a_store_draft set f_qty=f_qty-:f_qty where f_id=:f_id");
    }
    db[":f_header"] = uuid;
    db.exec("delete from o_goods where f_header=:f_header");
    //NONE COMPONNENTS EXIT (W/O COMPLECTATION)
    db[":f_header"] = uuid;
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_VOID;
    db.exec("select b.f_id, r.f_goods, r.f_qty*b.f_qty1 as f_totalqty, r.f_qty, "
            "g.f_lastinputprice, b.f_store "
            "from o_body b "
            "inner join d_recipes r on r.f_dish=b.f_dish "
            "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=0 "
            "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) ");
    goods.clear();
    while (db.nextRow()) {
        tmpg t;
        t.recId = db.getString("f_id");
        t.goodsId = db.getInt("f_goods");
        t.qtyGoods = db.getDouble("f_totalqty");
        t.store = db.getInt("f_store");
        t.price = db.getDouble("f_lastinputprice");
        goods.append(t);
    }
    //COMPONENT EXIT WITH COMPLECTATON
    db[":f_header"] = uuid;
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_VOID;
    db.exec("select b.f_id, gc.f_goods, ((gc.f_qty/g.f_complectout) * r.f_qty)*b.f_qty1 as f_totalqty, gc.f_qty, "
            "g.f_lastinputprice, b.f_store "
            "from o_body b "
            "inner join d_recipes r on r.f_dish=b.f_dish "
            "inner join c_goods_complectation gc on gc.f_base=r.f_goods "
            "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=1 "
            "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) ");
    while (db.nextRow()) {
        tmpg t;
        t.recId = db.getString("f_id");
        t.goodsId = db.getInt("f_goods");
        t.qtyGoods = db.getDouble("f_totalqty");
        t.store = db.getInt("f_store");
        t.price = db.getDouble("f_lastinputprice");
        goods.append(t);
    }
    //DISHES SET
    db[":f_header"] = uuid;
    db[":f_state1"] = DISH_STATE_OK;
    db[":f_state2"] = DISH_STATE_VOID;
    db.exec("select b.f_id, r.f_goods, r.f_qty*b.f_qty1*ds.f_qty as f_totalqty, r.f_qty, "
            "g.f_lastinputprice, b.f_store "
            "from o_body b "
            "inner join d_dish_set ds on ds.f_dish=b.f_dish "
            "inner join d_recipes r on r.f_dish=ds.f_part "
            "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=0 "
            "where b.f_header=:f_header and (b.f_state=:f_state1 or b.f_state=:f_state2) ");
    while (db.nextRow()) {
        tmpg t;
        t.recId = db.getString("f_id");
        t.goodsId = db.getInt("f_goods");
        t.qtyGoods = db.getDouble("f_totalqty");
        t.store = db.getInt("f_store");
        t.price = db.getDouble("f_lastinputprice");
        goods.append(t);
    }
    //WRITE RESULT OF RECIPE
    int row = 1;
    foreach (const tmpg &t, goods) {
        db[":f_id"] = db.uuid();
        db[":f_header"] = uuid;
        db[":f_body"] = t.recId;
        db[":f_goods"] = t.goodsId;
        db[":f_qty"] = t.qtyGoods;
        db[":f_store"] = t.store;
        db[":f_price"] = t.price;
        db[":f_total"] = t.price *t.qtyGoods;
        db[":f_row"] = row++;
        db[":f_tax"] = 0;
        db[":f_taxdept"] = 0;
        db.insert("o_goods", false);
    }
    if (!writeFromShopOutput(uuid, storedocstate, err)) {
        err += fErrorMsg + "<br>";
        return false;
    }
    return true;
}

bool C5StoreDraftWriter::transferToHotel(C5Database &db, const QString &uuid, QString &err)
{
#ifdef SMART
    Q_UNUSED(db);
    Q_UNUSED(uuid);
    err = "";
    return true;
#else
    fDb["f_id"] = uuid;
    fDb.exec("select * from o_header where f_id=:f_id");
    if (!fDb.nextRow()) {
        err = "o_header record not exists";
        return false;
    }
    QMap<QString, QVariant> fHeader;
    fDb.rowToMap(fHeader);
    if (fHeader["f_state"].toInt() != ORDER_STATE_CLOSE) {
        err = tr("Order state is not closed");
        return false;
    }
    fDb[":f_order"] = uuid;
    QJsonObject fTax;
    fDb.exec("select * from o_tax_log where f_order=:f_order and f_state=1");
    if (fDb.nextRow()) {
        fTax = __strjson(fDb.getString("f_out"));
    }
    int settings = 0;
    int item = 0;
    QString itemName;
    db[":f_id"] = fHeader["f_table"].toInt();
    db.exec("select f_special_config from h_tables where f_id=:f_id");
    if (db.nextRow()) {
        if (db.getInt(0) > 0) {
            settings = db.getInt(0);
        }
    }
    if (settings == 0) {
        db[":f_id"] = fHeader["f_hall"].toInt();
        db.exec("select f_settings from h_halls where f_id=:f_id");
        if (db.nextRow()) {
            settings = db.getInt(0);
        }
    }
    if (settings == 0) {
        err = "Cannot get settings for hall";
        return false;
    }
    db[":f_settings"] = settings;
    db[":f_key"] = param_item_code_for_hotel;
    db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (db.nextRow()) {
        item = db.getString("f_value").toInt();
    }
    if (item == 0) {
        err = "Cannot retrieve invoice item for hotel #1";
        return false;
    }
    bool no_hotel_invoice = false;
    db[":f_settings"] = settings;
    db[":f_key"] = param_hotel_noinvoice;
    db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (db.nextRow()) {
        no_hotel_invoice = db.getString("f_value").toInt() > 0;
    }
    int hallid = 0;
    db[":f_settings"] = settings;
    db[":f_key"] = param_hotel_hall_id;
    db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (db.nextRow()) {
        hallid = db.getString("f_value").toInt();
    }
    if (hallid == 0) {
        err = "Hall id undefined";
        return true;
    }
    int staffid = 0;
    db[":f_settings"] = settings;
    db[":f_key"] = param_hotel_user_Id;
    db.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
    if (db.nextRow()) {
        staffid = db.getString("f_value").toInt();
    }
    if (staffid == 0) {
        err = "Staff id undefined";
        return true;
    }
    if (!err.isEmpty()) {
        return false;
    }
    int paymentMode = 1;
    QString dc = "DEBET";
    int sign = -1;
    QString paymentModeComment;
    QString room, res, inv, clcode, clname, guest;
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_room where f_id=:f_id");
    if (db.nextRow()) {
        if (!db.getString("f_inv").isEmpty()) {
            room = db.getString("f_room");
            inv = db.getString("f_inv");
            res = db.getString("f_res");
            guest = db.getString("f_guest");
            paymentModeComment = QString("%1, %2").arg(room, guest);
            paymentMode = 5;
            dc = "CREDIT";
            sign = 1;
        }
    }
    if (fHeader["f_otherid"].toInt() == PAYOTHER_COMPLIMENTARY) {
        paymentMode = 6;
    } else if (fHeader["f_otherid"].toInt() == PAYOTHER_PRIMECOST) {
        paymentMode = 14;
    }
    if (fHeader["f_otherid"].toInt() == PAYOTHER_CL) {
        clcode = fHeader["f_other_clcode"].toString();
        clname = fHeader["f_other_clname"].toString();
        paymentMode = 4;
        dc = "DEBIT";
        sign = -1;
    }
    db[":f_id"] = fHeader["f_id"].toString();
    db.exec("select * from o_pay_cl where f_id=:f_id");
    if (db.nextRow()) {
        if (db.getInt("f_code") > 0) {
            clcode = db.getString("f_code");
            clname = db.getString("f_name");
            paymentMode = 4;
            dc = "DEBIT";
            sign = -1;
        }
    }
    if (fHeader["f_amountcard"].toDouble() > 0.001) {
        paymentMode = 2;
    }
    if (fHeader["f_amountbank"].toDouble() > 0.001) {
        paymentMode = 3;
    }
    QString result = QString("%1%2").arg(fHeader["f_prefix"].toString(), QString::number(fHeader["f_hallid"].toInt()));
    //Remove old
    db[":f_header"] = result;
    db.exec(QString("delete from %1.o_dish where f_header=:f_header").arg(__c5config.hotelDatabase()));
    db[":f_id"] = result;
    db.exec(QString("delete from %1.o_header where f_id=:f_id").arg(__c5config.hotelDatabase()));
    db[":f_id"] = result;
    db.exec(QString("delete from %1.m_register where f_id=:f_id").arg(__c5config.hotelDatabase()));
    db[":f_id"] = item;
    db.exec(QString("select f_en from %1.f_invoice_item where f_id=:f_id").arg(__c5config.hotelDatabase()));
    if (db.nextRow()) {
        itemName = db.getString(0);
    } else {
        err = QString("Cannot retrieve invoice item for hotel #2 %1").arg(item);
        return false;
    }
    QString payComment = "CASH";
    if (fHeader["f_amountcard"].toDouble() > 0.001) {
        payComment = C5CafeCommon::creditCardName(fHeader["f_creditcardid"].toInt());
    }
    if (!clcode.isEmpty()) {
        payComment = clname;
    }
    db[":f_id"] = result;
    db[":f_source"] = "PS";
    db[":f_res"] = no_hotel_invoice ? "" : res;
    db[":f_wdate"] = QDate::fromString(fHeader["f_datecash"].toString(), ("yyyy-MM-dd"));
    db[":f_rdate"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_user"] = staffid;
    db[":f_room"] = fHeader["f_otherid"].toInt() == PAYOTHER_TRANSFER_TO_ROOM ? room : clcode;
    db[":f_guest"] = fHeader["f_otherid"].toInt() == PAYOTHER_TRANSFER_TO_ROOM ? guest : clname + ", " +
                     fHeader["f_prefix"].toString() +
                     QString::number(fHeader["f_hallid"].toInt());
    db[":f_itemCode"] = item;
    db[":f_finalName"] = itemName + " " + result;
    db[":f_amountAmd"] = fHeader["f_amounttotal"].toDouble();
    db[":f_usedPrepaid"] = 0;
    db[":f_amountVat"] = fHeader["f_amounttotal"].toDouble() - (fHeader["f_amounttotal"].toDouble() / 1.2);
    db[":f_amountUsd"] = 0;
    db[":f_fiscal"] = fTax["rseq"].toInt();
    db[":f_fiscaldate"] = QDate::fromString(fHeader["f_dateclose"].toString(), "yyyy-MM-dd");
    db[":f_fiscaltime"] = QTime::fromString(fHeader["f_timeclose"].toString(), "HH:mm:ss");
    db[":f_paymentMode"] = paymentMode;
    db[":f_creditCard"] = fHeader["f_creditcardid"].toInt();
    db[":f_cityLedger"] = clcode.toInt();
    db[":f_paymentComment"] = payComment;
    db[":f_dc"] = dc;
    db[":f_sign"] = sign;
    db[":f_doc"] = "";
    db[":f_rec"] = "";
    db[":f_inv"] = no_hotel_invoice ? "" : inv;
    db[":f_vatmode"] = 1;
    db[":f_finance"] = 1;
    db[":f_remarks"] = "";
    db[":f_canceled"] = 0;
    db[":f_cancelReason"] = "";
    db[":f_cancelDate"] = 0;
    db[":f_cancelUser"] = 0;
    db[":f_side"] = 0;
    if (!db.insert(QString("%1.m_register").arg(__c5config.hotelDatabase()), false)) {
        err = "Cannot insert into m_register<br>" + db.fLastError;
        return false;
    }
    db[":f_id"] = result;
    db[":f_state"] = 2;
    db[":f_hall"] = hallid;
    db[":f_table"] = 1;
    db[":f_staff"] = staffid;
    db[":f_dateopen"] = QDateTime::fromString(fHeader["f_dateopen"].toString() + " " + fHeader["f_timeopen"].toString(),
                        "yyyy-MM-dd HH:mm:ss");
    db[":f_dateclose"] = QDateTime::fromString(fHeader["f_dateclose"].toString() + " " + fHeader["f_timeclose"].toString(),
                         "yyyy-MM-dd HH:mm:ss");
    db[":f_datecash"] = QDate::fromString(fHeader["f_datecash"].toString(), ("yyyy-MM-dd"));
    db[":f_comment"] = "";
    db[":f_paymentModeComment"] = paymentModeComment;
    db[":f_paymentMode"] = paymentMode;
    db[":f_cityLedger"] = clcode.toInt();
    db[":f_reservation"] = no_hotel_invoice ? "" : res;
    db[":f_complex"] = 0;
    db[":f_print"] = fHeader["f_print"].toInt();
    db[":f_tax"] = fTax["rseq"].toInt();
    db[":f_roomComment"] = "";
    db[":f_total"] = fHeader["f_total"].toDouble();
    if (!db.insert(QString("%1.o_header").arg(__c5config.hotelDatabase()), false)) {
        err = "Cannot insert into o_header<br>" + db.fLastError;
        return false;
    }
    QJsonArray fItems;
    db[":f_header"] = fHeader["f_id"].toString();
    db.exec("select ob.f_id, ob.f_header, ob.f_state, dp1.f_name as part1, dp2.f_name as part2, ob.f_adgcode, d.f_name as f_name, \
            ob.f_qty1, ob.f_qty2, ob.f_price, ob.f_service, ob.f_discount, ob.f_total, \
                        ob.f_store, ob.f_print1, ob.f_print2, ob.f_comment, ob.f_remind, ob.f_dish, \
                       s.f_name as f_storename, ob.f_removereason, ob.f_timeorder, ob.f_package, d.f_hourlypayment, \
                               ob.f_canservice, ob.f_candiscount, ob.f_guest \
            from o_body ob \
            left join d_dish d on d.f_id=ob.f_dish \
              left join d_part2 dp2 on dp2.f_id=d.f_part \
              left join d_part1 dp1 on dp1.f_id=dp2.f_part \
              left join c_storages s on s.f_id=ob.f_store \
              where ob.f_header=:f_header");
    while (db.nextRow()) {
        QMap<QString, QVariant> h;
        db.rowToMap(h);
        fItems.append(QJsonObject::fromVariantMap(h));
    }
    for (int i = 0; i < fItems.count(); i++) {
        QJsonObject o = fItems.at(i).toObject();
        if (o["f_state"].toInt() != DISH_STATE_OK) {
            continue;
        }
        double price = o["f_price"].toDouble();
        if (o["f_service"].toDouble() > 0.001) {
            price += price *o["f_service"].toDouble();
        }
        if (o["f_discount"].toDouble() > 0.001) {
            price -= price *o["f_discount"].toDouble();
        }
        db[":f_id"] = getHotelID("DR", err);
        db[":f_state"] = 1;
        db[":f_header"] = result;
        db[":f_dish"] = o["f_dish"].toInt();
        db[":f_qty"] = o["f_qty1"].toDouble();
        db[":f_qtyprint"] = o["f_qty2"].toDouble();
        db[":f_price"] = price;
        db[":f_svcvalue"] = o["f_service"].toDouble();
        db[":f_svcamount"] = 0;
        db[":f_dctvalue"] = 0;
        db[":f_dctamount"] = 0;
        db[":f_total"] = o["f_qty1"].toDouble() * price;
        db[":f_totalusd"] = 0;
        db[":f_print1"] = "";
        db[":f_print2"] = "";
        db[":f_store"] = o["f_store"].toInt();
        db[":f_comment"] = "";
        db[":f_staff"] = 1;
        db[":f_complex"] = 0;
        db[":f_complexid"] = 0;
        db[":f_adgt"] = o["f_adgcode"].toString();
        db[":f_complexRec"] = 0;
        db[":f_canceluser"] = 0;
        if (!db.insert(QString("%1.o_dish").arg(__c5config.hotelDatabase()), false)) {
            err = "Cannot insert into o_dish<br>" + db.fLastError;
            return false;
        }
    }
    db[":f_comp"] = QHostInfo::localHostName().toUpper();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_user"] = 1;
    db[":f_type"] = 23;
    db[":f_rec"] = QString("%1%2").arg(fHeader["f_prefix"].toString(),
                                       QString::number(fHeader["f_hallid"].toInt()));
    db[":f_invoice"] = inv;
    db[":f_reservation"] = res;
    db[":f_action"] = "IMPORT FROM WAITER";
    db[":f_value1"] = QString("%1%2").arg(fHeader["f_prefix"].toString(), QString::number(fHeader["f_hallid"].toInt()));
    db[":f_value2"] = fHeader["f_amounttotal"].toDouble();
    db.insert("airlog.log", false);
    return true;
#endif
}

QString C5StoreDraftWriter::getHotelID(const QString &source, QString &err)
{
#ifdef SMART
    Q_UNUSED(source);
    err = "";
    return "";
#else
    QStringList dbparams = __c5config.dbParams();
    dbparams[1] = "airwick";
    C5Database dba(dbparams);
    int totaltrynum = 0;
    bool done = false;
    QString result;
    do {
        QString query = QString ("select f_max, f_zero from airwick.serv_id_counter where f_id='%1' for update").arg(source);
        if (!dba.exec(query)) {
            err = "<H1><font color=\"red\">ID ERROR. COUNTER ID GENERATOR FAIL</font></h1><br>" + dba.fLastError;
            exit(0);
        }
        if (dba.nextRow()) {
            int max = dba.getInt(0) + 1;
            int zero = dba.getInt(1);
            dba[":f_max"] = max;
            dba[":f_id"] = source;
            query = "update airwick.serv_id_counter set f_max=:f_max where f_id=:f_id";
            dba.exec(query);
            result = QString("%1").arg(max, zero, 10, QChar('0'));
        } else {
            query = "insert into airwick.serv_id_counter (f_id, f_max, f_zero) values ('" + source + "', 0, 6)";
            dba.exec(query);
            totaltrynum++;
            continue;
        }
        dba.exec(QString("insert into airwick.f_id (f_value, f_try, f_comp, f_user, f_date, f_time, f_db) values ('%1-%2', %3, '%4', '%5', '%6', '%7', database())")
                 .arg(source)
                 .arg(result).arg(totaltrynum)
                 .arg(QHostInfo::localHostName().toUpper())
                 .arg(1) //user id
                 .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                 .arg(QTime::currentTime().toString("HH:mm:ss")));
        if (dba.fLastError.toLower().contains("duplicate entry")) {
            totaltrynum++;
        } else {
            done = true;
        }
        if (totaltrynum > 20) {
            err = "<H1><font color=\"red\">ID ERROR, TRYNUM>20. COUNTER ID GENERATOR FAIL, GIVE UP</font></h1>";
            exit(0);
        }
    } while (!done);
    result = source + result;
    return result;
#endif
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

bool C5StoreDraftWriter::writeACalcPrice(const QString &id, const QString &docId, double price2, double margin)
{
    fDb[":f_id"] = id;
    fDb[":f_document"] = docId;
    fDb[":f_price2"] = price2;
    fDb[":f_margin"] = margin;
    return returnResult(fDb.insert("a_calc_price", false));
}

bool C5StoreDraftWriter::writeAStoreInventory(QString &id, const QString &docId, int store, int goods, double qty,
        double price, double total)
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

bool C5StoreDraftWriter::writeAStoreDishWaste(QString &id, const QString &docId, int dish, double qty,
        const QString &data)
{
    bool u = true;
    if (id.isEmpty()) {
        id = fDb.uuid();
        u = false;
    }
    fDb[":f_id"] = id;
    fDb[":f_document"] = docId;
    fDb[":f_dish"] = dish;
    fDb[":f_qty"] = qty;
    fDb[":f_data"] = data;
    if (u) {
        return returnResult(fDb.update("a_store_dish_waste", "f_id", id));
    } else {
        return returnResult(fDb.insert("a_store_dish_waste", false));
    }
}

bool C5StoreDraftWriter::writeECash(QString &id, const QString &header, int cash, int sign, const QString &purpose,
                                    double amount, QString &base, int rownum)
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
    fDb[":f_working_session"] = __c5config.getRegValue("sessionid");
    if (u) {
        return returnResult(fDb.update("e_cash", where_id(id)));
    } else {
        return returnResult(fDb.insert("e_cash", false));
    }
}

bool C5StoreDraftWriter::writeOBody(QString &id, const QString &header, int state, int dish, double qty1, double qty2,
                                    double price, double total, double service, double discount,
                                    int store, const QString &print1, const QString &print2, const QString &comment, int remind, const QString &adgcode,
                                    int removereason, int timeorder, int package, int row, const QDateTime &appendTime, const QString &emarks)
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
    fDb[":f_row"] = row;
    fDb[":f_emarks"] = emarks;
    fDb[":f_working_day"] = __c5config.dateCash();
    if (u) {
        return returnResult(fDb.update("o_body", where_id(id)));
    } else {
        fDb[":f_appendtime"] = appendTime;
        return returnResult(fDb.insert("o_body", false));
    }
}

bool C5StoreDraftWriter::writeOBodyToOGoods(const QString &id, const QString &headerid)
{
    fDb[":f_body"] = id;
    fDb.exec("delete from o_goods where f_body=:f_body");
    fDb[":f_id"] = id;
    fDb.exec("select r.f_goods, b.f_store, b.f_qty1*r.f_qty as f_qty, g.f_lastinputprice as f_price "
             "from d_recipes r "
             "inner join o_body b on b.f_dish=r.f_dish "
             "inner join c_goods g on g.f_id=r.f_goods "
             "where b.f_id=:f_id");
    int row = 0;
    OGoods g;
    QString err;
    while (g.getRecord(fDb)) {
        g.row = ++row;
        g.header = headerid;
        g.body = id;
        g.total = g.qty *g.price;
        g.write(fDb, err);
    }
    return true;
}

bool C5StoreDraftWriter::writeOHeaderFlags(const QString &id, int f1, int f2, int f3, int f4, int f5)
{
    fDb[":f_1"] = f1;
    fDb[":f_2"] = f2;
    fDb[":f_3"] = f3;
    fDb[":f_4"] = f4;
    fDb[":f_5"] = f5;
    return returnResult(fDb.update("o_header_flags", "f_id", id));
}

bool C5StoreDraftWriter::writeOHeaderOptions(const QString &id, int guest, int splitted, int deliveryman, int currency)
{
    fDb[":f_guests"] = guest;
    fDb[":f_splitted"] = splitted;
    fDb[":f_deliveryman"] = deliveryman;
    fDb[":f_currency"] = currency;
    return returnResult(fDb.update("o_header_options", "f_id", id));
}

bool C5StoreDraftWriter::writeOPackage(int &id, const QString &header, int package, double qty, double price)
{
    fDb[":f_id"] = id;
    fDb[":f_header"] = header;
    fDb[":f_package"] = package;
    fDb[":f_qty"] = qty;
    fDb[":f_price"] = price;
    if (id == 0) {
        id = fDb.insert("o_package");
        if (id == 0) {
            fErrorMsg = fDb.fLastError;
            return false;
        }
    } else {
        return returnResult(fDb.update("o_package", "f_id", id));
    }
    return true;
}

bool C5StoreDraftWriter::writeInput(const QString &docId, QString &err)
{
    double total = 0;
    fDb[":f_document"] = docId;
    fDb.exec("select * from a_store_draft where f_document=:f_document");
    QString longsql =
        "insert into a_store (f_id, f_document, f_store, f_type, f_goods, "
        "f_qty, f_price, f_total, "
        "f_base, f_basedoc, f_reason, f_draft) values ";
    bool f = true;
    while (fDb.nextRow()) {
        if (f) {
            f = false;
        } else {
            longsql.append(",");
        }
        longsql.append(QString("('%1', '%2', %3, %4, %5, %6, %7, %8, '%9', '%10', %11, '%12')")
                       .arg(fDb.getString("f_id"))
                       .arg(fDb.getString("f_document"))
                       .arg(fDb.getInt("f_store"))
                       .arg(fDb.getInt("f_type"))
                       .arg(fDb.getInt("f_goods"))
                       .arg(fDb.getDouble("f_qty"))
                       .arg(fDb.getDouble("f_price"))
                       .arg(fDb.getDouble("f_total"))
                       .arg(fDb.getString("f_id"))
                       .arg(docId)
                       .arg(fDb.getInt("f_reason"))
                       .arg(fDb.getString("f_id")));
        total += fDb.getDouble("f_total");
    }
    if (!fDb.exec(longsql)) {
        err += fDb.fLastError + "<br>";
        return false;
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
        return false;
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
    fDb.exec("select s.f_id, g.f_storeid as f_goods, s.f_store, s.f_qty, s.f_price, s.f_total, "
             "concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, s.f_base, s.f_reason "
             "from a_store_draft s "
             "inner join c_goods g on g.f_id=s.f_goods "
             "where f_document=:f_document and f_type=-1");
    while (fDb.nextRow()) {
        recID.append(fDb.getString("f_id"));
        baseID.append(fDb.getString("f_base"));
        goodsID.append(QString::number(fDb.getInt("f_goods")));
        goodsName.append(fDb.getString("f_name"));
        reason = fDb.getInt("f_reason");
        qtyList.append(fDb.getDouble("f_qty"));
        priceList.append(fDb.getDouble("f_price"));
        totalList.append(fDb.getDouble("f_total"));
    }
    if (goodsID.isEmpty()) {
        err = tr("Emtpy document");
        return false;
    }
    QList<QMap<QString, QVariant> > storeData;
    fDb[":f_store"] = storeOut;
    if (!fDb.exec(
                QString("select s.f_id, s.f_goods, sm.f_qty, sm.f_price, sm.f_total, "
                        "s.f_document, s.f_base, s.f_basedoc, d.f_date "
                        "from a_store s "
                        "left join ( "
                        "select   sum(ss.f_qty*ss.f_type) as f_qty, ss.f_price, "
                        "sum(ss.f_total*ss.f_type) as f_total, ss.f_base "
                        "from a_store ss "
                        "where ss.f_goods in (%1) and ss.f_store=:f_store "
                        "group by ss.f_base  "
                        "having sum(ss.f_qty*ss.f_type)>0 "
                        ") sm on sm.f_base=s.f_id "
                        "inner join a_header d on d.f_id=s.f_document "
                        "where s.f_goods in (%1) and s.f_store=:f_store and s.f_type=1 and sm.f_qty>0 "
                        "order by d.f_date "
                        "for update ").arg(goodsID.join(",")))) {
        err = fDb.fLastError + "<br>";
        return false;
    }
    while (fDb.nextRow()) {
        if (fDb.getDate("f_date") > date) {
            continue;
        }
        QMap<QString, QVariant> v;
        fDb.rowToMap(v);
        storeData.append(v);
    }
    QList<QMap<QString, QVariant> > queries;
    for (int i = 0; i < goodsID.count(); i++) {
        double qty = qtyList.at(i);
        totalList[i] = 0;
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j)["f_goods"].toInt() == goodsID.at(i).toInt()) {
                if (storeData.at(j)["f_qty"].toDouble() > 0) {
                    if (storeData.at(j)["f_qty"].toDouble() >= qty) {
                        storeData[j]["f_qty"] = storeData.at(j)["f_qty"].toDouble() - qty;
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j)["f_price"];
                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j)["f_base"];
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        amount += storeData.at(j)["f_price"].toDouble() * qty;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_price"].toDouble() * qty);
                        priceList[i] = totalList[i] / qtyList.at(i);
                        qty = 0;
                    } else {
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = storeData.at(j)["f_qty"];
                        newrec[":f_price"] = storeData.at(j)["f_price"];
                        newrec[":f_total"] = storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble();
                        newrec[":f_base"] = storeData.at(j)["f_base"];
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        queries << newrec;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_price"].toDouble() * storeData.at(j)["f_qty"].toDouble());
                        priceList[i] = totalList[i] / qtyList.at(i);
                        amount += storeData.at(j)["f_price"].toDouble() * storeData.at(j)["f_qty"].toDouble();
                        qty -= storeData.at(j)["f_qty"].toDouble();
                        storeData[j]["f_qty"] = 0.0;
                    }
                }
            }
            if (qty < 0.00001) {
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
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.constBegin(); it != queries.constEnd(); it++) {
            QString newId = C5Database::uuid();
            outId << newId;
            fDb.setBindValues( *it);
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
                        QString mid = C5Database::uuid();
                        fDb[":f_base"] = mid;
                        fDb[":f_basedoc"] = fDb[":f_id"];
                        fDb[":f_id"] = mid;
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
                    fDb[":f_base"] = id;
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
        if (!outputRollback(fDb, id)) {
            err = fDb.fLastError;
        }
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
