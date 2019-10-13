#ifndef C5STOREDOCHANDLER_H
#define C5STOREDOCHANDLER_H

#include "c5database.h"

struct StoreGoods {
    QString fUuid;
    int fGoods;
    double fQty;
    int fReason;
    int fStorein;
    int fStoreout;
    QString fRelUuid;
    StoreGoods() {}
    StoreGoods(C5Database &db) {
        fUuid = db.getString("f_id");
        fGoods = db.getInt("f_goods");
        fQty = db.getDouble("f_qty");
    }
};

class C5StoreDocHandler
{
public:
    C5StoreDocHandler(C5Database &db);

    void openDoc(const QString &id);

    const QString appendToDraft(const StoreGoods &sg);

    void writeDraft(const QDate &date, int doctype, int storein, int storeout, int reason, const QMap<int, double> &data, const QString &comment);

    QString fDocumentId;

private:
    C5Database &fDb;

    QList<StoreGoods> fGoodsList;
};

#endif // C5STOREDOCHANDLER_H
