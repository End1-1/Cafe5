#pragma once

#include <QDate>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

class C5User;
class QObject;
struct GoodsItem;
struct PartnerItem;

struct StoreInputXmlGoodLine
{
    QString description;
    double qty = 0;
    /** Unit price including VAT (TotalPrice / Amount from XML). */
    double pricePerUnit = 0;
    /** Line total including VAT (XML TotalPrice). */
    double totalPrice = 0;
    QString unit;
    QString classifierCode;
};

struct StoreInputXmlInvoice
{
    QString tin;
    QString supplierName;
    QString supplierAddress;
    QString invoiceSeries;
    QString invoiceNumber;
    QString docNumber;
    QDate supplyDate;
    QString additionalData;
    double totalPrice = 0;
    QVector<StoreInputXmlGoodLine> goods;
};

/** Pre-import policy chosen in DlgStoreInputXmlImportOptions. */
struct StoreInputXmlImportOptions
{
    enum GoodsMissingAction {
        GoodsAutoCreate = 0,
        GoodsMatch = 1
    };
    enum PartnerMissingAction {
        PartnerAutoCreate = 0,
        PartnerDoNotCreate = 1
    };
    enum DuplicateAction {
        DupOverwrite = 0,
        DupSkip = 1,
        DupAbort = 2
    };
    enum SaveMode {
        SaveOpenInWindows = 0,
        SaveImmediateDraft = 1,
        SaveImmediatePosted = 2
    };

    GoodsMissingAction goodsMissing = GoodsMatch;
    PartnerMissingAction partnerMissing = PartnerAutoCreate;
    DuplicateAction duplicates = DupOverwrite;
    int storeId = 0;
    QString storeName;
    SaveMode saveMode = SaveOpenInWindows;

    bool saveImmediately() const { return saveMode != SaveOpenInWindows; }
};

struct StoreInputXmlGoodsMappingRow
{
    StoreInputXmlGoodLine source;
    int goodsId = 0;
    QString goodsName;
    QString goodsUnit;
    QString goodsAdgt;
    bool updateName = false;
    bool createNew = false;
};

class StoreInputXmlImport
{
public:
    static QString normalizeName(const QString &name);

    static QVector<StoreInputXmlInvoice> parseFile(const QString &path, QString &error);

    static bool httpQuery(C5User *user,
                          QObject *context,
                          const QString &route,
                          const QJsonObject &params,
                          QJsonObject &response,
                          QString &error);

    static bool findPartnerByTin(C5User *user,
                                 QObject *context,
                                 const QString &tin,
                                 QJsonObject &partner,
                                 QString &error);

    static bool createPartner(C5User *user,
                              QObject *context,
                              const QString &tin,
                              const QString &taxName,
                              const QString &address,
                              QJsonObject &partner,
                              QString &error);

    static bool updatePartner(C5User *user, QObject *context, const QJsonObject &partner, QString &error);

    static bool findGoodsByExactName(C5User *user,
                                     QObject *context,
                                     const QString &description,
                                     GoodsItem &goods,
                                     QString &error);

    static bool findGoodsById(C5User *user,
                              QObject *context,
                              int goodsId,
                              GoodsItem &goods,
                              QString &error);

    static bool createGoodsFromXml(C5User *user,
                                   QObject *context,
                                   int supplierId,
                                   const StoreInputXmlGoodLine &line,
                                   GoodsItem &goods,
                                   QString &error);

    static bool renameGoods(C5User *user, QObject *context, int goodsId, const QString &name, QString &error);

    static bool findExistingByInvoiceNumber(C5User *user,
                                            QObject *context,
                                            const QString &series,
                                            const QString &number,
                                            QJsonArray &docs,
                                            QString &error);

    static bool removeDocument(C5User *user, QObject *context, const QString &id, QString &error);
};
