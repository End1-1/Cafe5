#ifndef C5STOREDRAFTWRITER_H
#define C5STOREDRAFTWRITER_H

#include "c5database.h"

#define container_ecash 1
#define container_aheader 2
#define container_aheadercash 3
#define container_aheaderstore 4
#define container_astoredraft 5

struct IGoods {
    QString recId;
    QString bodyId;
    int goodsId;
    int tax;
    int row;
    QString goodsName;
    double goodsQty;
    double goodsPrice;
    double goodsTotal;
    double discountFactor;
    int discountMode;
    int store;
    int unitId;
    bool changed;
    QString taxDept;
    QString taxAdg;
    bool isService;
    int storeId;
    IGoods() {changed = false; }
};

class C5StoreDraftWriter : QObject
{
    Q_OBJECT

public:
    C5StoreDraftWriter(C5Database &db);

    bool writeAHeader(QString &id, const QString &userid, int state, int type, int op, const QDate &docDate, const QDate &dateCreate, const QTime &timeCreate, int partner, double amount, const QString &comment, int payment, int paid);

    bool writeAHeaderPartial(QString &id, const QString &userid, int op, const QDate &dateCreate, const QTime &timeCreate, int partner, const QString &comment, int payment, int paid);

    bool writeAHeaderStore(const QString &id, int userAccept, int userPass, const QString &invoice, const QDate &invoiceDate, int storeIn, int storeOut, int basedOnSale, const QString &cashUUID, int complectation, double complectationQty);

    bool writeAHeaderCash(const QString &id, int cashin, int cashout, int related, const QString &storedoc, const QString &oheader);

    bool writeAHeader2ShopStore(const QString &id, int store, int accept);

    bool writeAStoreDraft(QString &id, const QString &docId, int store, int type, int goods, double qty, double price, double total, int reason, const QString &baseid, int rownum, const QString &comment);

    bool writeAStoreInventory(QString &id, const QString &docId, int store, int goods, double qty, double price, double total);

    bool writeBHistory(const QString &id, int type, int card, double value, double data);

    bool writeECash(QString &id, const QString &header, int cash, int sign, const QString &purpose, double amount, QString &base, int rownum);

    bool writeOBody(QString &id, const QString &header, int state, int dish, double qty1, double qty2, double price, double total, double service, double discount, int store, const QString &print1, const QString &print2, const QString &comment, int remind, const QString &adgcode, int removereason, int timeorder, int package);

    bool writeOBodyToOGoods(const QString &id, const QString &headerid);

    bool writeOHeader(QString &id, int hallid, const QString &prefix, int state, int hall, int table, const QDate &dateopen, const QDate &dateclose, const QDate &datecash, const QTime &timeopen, const QTime &timeclose, int staff, const QString &comment, int print, double amountTotal, double amountCash, double amountCard, double amountBank, double amountOther, int serviceMode, double amountService, double amountDiscount, double serviceFactor, double discountFactor, int creditCardId, int otherId, int shift, int source, int saletype, int partner);

    bool writeOGoods(QString &id, const QString &header, const QString &body, int store, int goods, double qty, double price, double total, int tax, int sign, int row, const QString &storerec, double discount, int discountMode, int returnMode, double discFactor);

    bool readAHeader(const QString &id);

    bool writeInput(const QString &docId, QString &err);

    bool writeOutput(const QString &docId, QString &err);

    bool writeTotalStoreAmount(const QString &docId);

    bool haveRelations(const QString &id, QString &err, bool clear);

    bool clearAStoreDraft(const QString &id);

    bool writeFromShopOutput(const QString &doc, int state, QString &err);

    bool rollbackOutput(C5Database &db, const QString &id);

    QString storeDocNum(int docType, int storeId, bool withUpdate, int value);

    bool hallId(QString &prefix, int &id, int hall);

    int counterAType(int type);

    bool updateField(const QString &tableName, const QString &fieldName, const QVariant &fieldValue, const QString &whereFieldName, const QVariant &whereFieldValue);

    QString fErrorMsg;

    int rowCount(int container);

    QVariant value(int container, int row, const QString &key);

private:
    C5Database &fDb;

    QList<QList<QVariant> > fAStoreDraftData;

    QHash<QString, int> fAStoreDraftDataMap;

    QList<QList<QVariant> > fAHeaderStoreData;

    QHash<QString, int> fAHeaderStoreDataMap;

    QList<QList<QVariant> > fAHeaderData;

    QHash<QString, int> fAHeaderDataMap;

    QList<QList<QVariant> > fAHeaderCashData;

    QHash<QString, int> fAHeaderCashDataMap;

    QList<QList<QVariant> > fECashData;

    QHash<QString, int> fECashDataMap;

    bool readAHeaderStore(const QString &id);

    bool readAStoreDraft(const QString &id);

    bool readAHeaderCash(const QString &id);

    bool readECash(const QString &id);

    bool returnResult(bool r, const QString &msg = "");

};

#endif // C5STOREDRAFTWRITER_H
