#pragma once

#include <QDate>
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
    double pricePerUnit = 0;
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

struct StoreInputXmlGoodsMappingRow
{
    StoreInputXmlGoodLine source;
    int goodsId = 0;
    QString goodsName;
    QString goodsUnit;
    QString goodsAdgt;
    bool updateName = false;
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

    static bool findGoodsByExactName(C5User *user, const QString &description, GoodsItem &goods, QString &error);

    static bool findGoodsById(int goodsId, GoodsItem &goods);

    static bool renameGoods(C5User *user, QObject *context, int goodsId, const QString &name, QString &error);
};
