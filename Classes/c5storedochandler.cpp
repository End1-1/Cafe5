#include "c5storedochandler.h"
#include "c5utils.h"
#include "c5config.h"
#include <QJsonDocument>
#include <QJsonObject>

C5StoreDocHandler::C5StoreDocHandler(C5Database &db) :
    fDb(db)
{

}

void C5StoreDocHandler::openDoc(const QString &id)
{
    fGoodsList.clear();
    fDocumentId = id;
    fDb[":f_document"] = fDocumentId;
    fDb.exec("select f_id, f_goods, f_qty from a_store_draft where f_document=:f_document");
    while (fDb.nextRow()) {
        StoreGoods sg(fDb);
        fGoodsList.append(sg);
    }
}

const QString C5StoreDocHandler::appendToDraft(const StoreGoods &sg)
{
    for (StoreGoods &s: fGoodsList) {
        if (s.fGoods == sg.fGoods) {
            fDb[":f_id"] = s.fUuid;
            fDb[":f_qty"] = sg.fQty;
            fDb.exec("update a_store_draft set f_qty=f_qty+:f_qty where f_id=:f_id");
            return s.fUuid;
        }
    }
    StoreGoods nsg = sg;
    nsg.fUuid = C5Database::uuid();
    fDb[":f_id"] = nsg.fUuid;
    fDb[":f_document"] = fDocumentId;
    fDb[":f_goods"] = nsg.fGoods;
    fDb[":f_qty"] = nsg.fQty;
    fDb[":f_price"] = 0;
    fDb[":f_total"] = 0;
    fDb[":f_reason"] = nsg.fReason;
    fDb[":f_storein"] = nsg.fStorein;
    fDb[":f_storeout"] = nsg.fStoreout;
    fDb.insert("a_store_draft", false);
    fGoodsList.append(nsg);
    return nsg.fUuid;
}

void C5StoreDocHandler::writeDraft(const QDate &date, int doctype, int storein, int storeout, int reason, const QMap<int, double> &data, const QString &comment)
{
    QJsonObject jo;
    switch (doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        jo["f_storein"] = "";
        jo["f_storeout"] = QString::number(storeout);
        break;
    case DOC_TYPE_STORE_INPUT:
        jo["f_storein"] = QString::number(storein);
        jo["f_storeout"] = "";
        break;
    }
    QJsonDocument jd(jo);
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
        fDb[":f_goods"] = it.key();
        fDb[":f_qty"] = it.value();
        fDb[":f_price"] = 0;
        fDb[":f_total"] = 0;
        fDb[":f_reason"] = reason;
        fDb[":f_storein"] = storein;
        fDb[":f_storeout"] = storeout;
        fDb.insert("a_store_draft", false);
    }
}
