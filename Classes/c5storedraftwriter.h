#ifndef C5STOREDRAFTWRITER_H
#define C5STOREDRAFTWRITER_H

#include "c5database.h"

#define container_ecash 1
#define container_aheader 2
#define container_aheadercash 3
#define container_aheaderstore 4
#define container_astoredraft 5
#define container_astoredishwaste 6
#define container_bclient_debts 8

class C5StoreDraftWriter : QObject
{
    Q_OBJECT

public:
    C5StoreDraftWriter(C5Database &db);

    bool writeAHeader(QString &id, const QString &userid, int state, int type, int op, const QDate &docDate,
                      const QDate &dateCreate, const QTime &timeCreate, int partner, double amount, const QString &comment, int paid,
                      int currency, int working_session = 0);

    bool writeAHeaderPartial(QString &id, const QString &userid, int op, const QDate &dateCreate, const QTime &timeCreate,
                             int partner, const QString &comment);

    bool writeAHeaderStore(const QString &id, int userAccept, int userPass, const QString &invoice,
                           const QDate &invoiceDate, int storeIn, int storeOut, int basedOnSale, const QString &cashUUID, int complectation,
                           double complectationQty, const QString &saleuuid);

    bool writeAHeaderCash(const QString &id, int cashin, int cashout, int related, const QString &storedoc,
                          const QString &oheader);

    bool writeAHeader2ShopStore(const QString &id, int store, int accept);

    bool writeAStoreDraft(QString &id, const QString &docId, int store, int type, int goods, double qty, double price,
                          double total, int reason, const QString &baseid, int rownum, const QString &comment);

    bool writeACalcPrice(const QString &id, const QString &docId, double price2, double margin);

    bool writeAStoreInventory(QString &id, const QString &docId, int store, int goods, double qty, double price,
                              double total);

    bool writeAStoreDishWaste(QString &id, const QString &docId, int dish, double qty, const QString &data);

    bool writeECash(QString &id, const QString &header, int cash, int sign, const QString &purpose, double amount,
                    QString &base, int rownum);

    bool writeOBody(QString &id, const QString &header, int state, int dish, double qty1, double qty2, double price,
                    double total, double service, double discount, int store, const QString &print1, const QString &print2,
                    const QString &comment, int remind, const QString &adgcode, int removereason, int timeorder, int package, int row,
                    const QDateTime &appendTime, const QString &emarks);

    bool writeOBodyToOGoods(const QString &id, const QString &headerid);

    bool writeOHeaderFlags(const QString &id, int f1, int f2, int f3, int f4, int f5);

    bool writeOHeaderOptions(const QString &id, int guest, int splitted, int deliveryman, int currency);

    bool writeOPackage(int &id, const QString &header, int package, double qty, double price);

    bool readAHeader(const QString &id);

    bool writeInput(const QString &docId, QString &err);

    bool writeOutput(const QString &docId, QString &err);

    bool writeTotalStoreAmount(const QString &docId);

    bool haveRelations(const QString &id, QString &err, bool clear);

    bool clearAStoreDraft(const QString &id);

    bool writeFromShopOutput(const QString &doc, int state, QString &err);

    bool rollbackOutput(C5Database &db, const QString &id);

    bool removeStoreDocument(C5Database &db, const QString &id, QString &err);

    bool outputRollback(C5Database &db, const QString &id);

    QString storeDocNum(int docType, int storeId, bool withUpdate, int value);

    bool hallId(QString &prefix, int &id, int hall);

    int counterAType(int type);

    bool updateField(const QString &tableName, const QString &fieldName, const QVariant &fieldValue,
                     const QString &whereFieldName, const QVariant &whereFieldValue);

    QString fErrorMsg;

    int rowCount(int container);

    QVariant value(int container, int row, const QString &key);

    bool writeStoreOfSale(const QString &uuid,  QString &err, int storedocstate);

    bool transferToHotel(C5Database &db, const QString &uuid, QString &err);

    QString getHotelID(const QString &source, QString &err);

private:
    C5Database &fDb;

    std::vector<QJsonArray > fAStoreDraftData;

    QHash<QString, int> fAStoreDraftDataMap;

    std::vector<QJsonArray > fAHeaderStoreData;

    QHash<QString, int> fAHeaderStoreDataMap;

    std::vector<QJsonArray > fAStoreDishWaste;

    QHash<QString, int> fAStoreDishWasteDataMap;

    std::vector<QJsonArray > fAHeaderData;

    QHash<QString, int> fAHeaderDataMap;

    std::vector<QJsonArray > fAHeaderCashData;

    QHash<QString, int> fAHeaderCashDataMap;

    std::vector<QJsonArray > fECashData;

    QHash<QString, int> fECashDataMap;

    QHash<QString, int> fBClientsDebtsDataMap;

    std::vector<QJsonArray > fBClientsDebtsData;

    bool readAHeaderStore(const QString &id);

    bool readAStoreDraft(const QString &id);

    bool readAStoreDishWaste(const QString &id);

    bool readAHeaderCash(const QString &id);

    bool readECash(const QString &id);

    bool readBClientDebtsRefund(const QString &id);

    bool returnResult(bool r, const QString &msg = "");

};

#endif // C5STOREDRAFTWRITER_H
