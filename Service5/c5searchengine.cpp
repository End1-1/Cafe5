#include "c5searchengine.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QWriteLocker>
#include "database.h"
#include "logwriter.h"
#include "socketstruct.h"
#include "store_doc_status.h"
#include "store_doc_type.h"
#include "struct_cashbox.h"
#include "struct_currency.h"
#include "struct_employee.h"
#include "struct_employee_group.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_payment_type.h"
#include "struct_storage_item.h"

static const QString mSqlGoods = R"(
SELECT 
    g.f_id, 
    g.f_group AS f_group_id, 
    gr.f_name AS f_group_name,
    g.f_name, 
    GROUP_CONCAT(gm.f_id SEPARATOR ', ') AS f_barcode,
    u.f_name AS f_unit_name,
    gp.f_price1, 
    gp.f_price1disc, 
    gp.f_price2, 
    gp.f_price2disc,
    IF(LENGTH(g.f_adg) > 0, g.f_adg, gr.f_adgcode) AS f_adgt
FROM c_goods g
LEFT JOIN c_goods_multiscancode gm ON gm.f_goods = g.f_id
LEFT JOIN c_groups gr ON gr.f_id = g.f_group
LEFT JOIN c_units u ON u.f_id = g.f_unit
LEFT JOIN c_goods_prices gp ON gp.f_goods = g.f_id AND gp.f_currency = 1
%where%
GROUP BY g.f_id
ORDER BY g.f_name
    )";

QReadWriteLock mStoragesLock;
QReadWriteLock mGoodsLock;
QReadWriteLock mPartnersLock;
QReadWriteLock mCurrencyLock;
QReadWriteLock mCashboxLock;
QReadWriteLock mPaymentTypeLock;
QReadWriteLock mEmployeeLock;
QReadWriteLock mEmployeeGroupLock;
QHash<QString, QVector<StorageItem>> mStorages;
QHash<QString, QVector<GoodsItem>> mGoods;
QHash<QString, QHash<int, int>> mGoodsIndex;
QHash<QString, QVector<PartnerItem>> mPartners;
QHash<QString, QVector<StoreDocStatusItem>> mStoreDocStatus;
QHash<QString, QVector<StoreDocTypeItem>> mStoreDocType;
QHash<QString, QVector<StructCurrency>> mCurrency;
QHash<QString, QVector<StructCashbox>> mCashbox;
QHash<QString, QVector<StructPaymentType>> mPaymentType;
QHash<QString, QVector<StructEmployee>> mEmployees;
QHash<QString, QVector<StructEmployeeGroup>> mEmployeeGroups;

struct SearchObject
{
    int mode;
    int objectId;
    QString name;
};
// Database - > List of words
QMap<QString, QStringList> mSearchStrings;
// Database -> Index of word -> SearchObject
QMap<QString, QMap<int, SearchObject>> mSearchObjects;
C5SearchEngine *C5SearchEngine::mInstance = nullptr;
QMap<QString, QJsonArray> mSearchPartners;
QMap<QString, QJsonArray> mSearchGoodsGroups;
QMap<QString, QJsonArray> mSearchGoods;

C5SearchEngine::C5SearchEngine()
    : QObject()
{}

void C5SearchEngine::init(QStringList databases)
{
    Database db;
    int totalitems = 0;
    mSearchObjects.clear();
    mSearchStrings.clear();
    mSearchPartners.clear();
    mSearchGoodsGroups.clear();
    mSearchGoods.clear();

    for (const QString &dbname : databases) {
        int dbitemscount = 0;
        int port = 3306;
#ifdef QT_DEBUG
        port = 3306;
#endif

        if (!db.open("127.0.0.1", dbname, "root", "root5", port)) {
            continue;
        }

        mSearchStrings[dbname] = QStringList();
        mSearchObjects[dbname] = QMap<int, SearchObject>();
        QStringList &words = mSearchStrings[dbname];
        QMap<int, SearchObject> &objects = mSearchObjects[dbname];
        QString sql = QString::fromStdString(R"sql(
        SELECT f_id, f_mode, f_word as f_orig, lower(f_word) as f_word FROM (
        SELECT d.f_id, d.f_mode, d.f_en AS f_word FROM d_part2 p2
            left join d_translator d on d.f_id=p2.f_id
            WHERE d.f_mode = 2 and p2.f_id not in (select distinct(f_parent) from d_part2 where f_parent>0)
        UNION
        SELECT d.f_id, d.f_mode, d.f_ru AS f_word FROM d_part2 p2
            left join d_translator d on d.f_id=p2.f_id
            WHERE d.f_mode = 2 and p2.f_id not in (select distinct(f_parent) from d_part2 where f_parent>0)
        union
        SELECT d.f_id, d.f_mode, d.f_am AS f_word FROM d_part2 p2
            left join d_translator d on d.f_id=p2.f_id
            WHERE d.f_mode = 2 and p2.f_id not in (select distinct(f_parent) from d_part2 where f_parent>0)
        UNION
        SELECT d1.f_id, d1.f_mode, CONCAT_WS(' ',d1.f_en, d2.f_en) AS f_word
        FROM d_menu m
        left join d_dish d on d.f_id=m.f_dish
        LEFT JOIN d_translator d1 ON d1.f_id=d.f_id AND d1.f_mode=1
        LEFT JOIN d_translator d2 ON d2.f_id=d.f_id AND d2.f_mode=3
        where m.f_state=1
        UNION
        SELECT d1.f_id, d1.f_mode, CONCAT_WS(' ',d1.f_ru, d2.f_ru) AS f_word
        FROM d_menu m
        left join d_dish d on d.f_id=m.f_dish
        LEFT JOIN d_translator d1 ON d1.f_id=d.f_id AND d1.f_mode=1
        LEFT JOIN d_translator d2 ON d2.f_id=d.f_id AND d2.f_mode=3
        where m.f_state=1
        UNION
        SELECT d1.f_id, d1.f_mode, CONCAT_WS(' ',d1.f_am, d2.f_am) AS f_word
        FROM d_menu m
        left join d_dish d on d.f_id=m.f_dish
        LEFT JOIN d_translator d1 ON d1.f_id=d.f_id AND d1.f_mode=1
        LEFT JOIN d_translator d2 ON d2.f_id=d.f_id AND d2.f_mode=3
        where m.f_state=1
        ) AS tr
        WHERE LENGTH(trim(f_word))>0
        ORDER BY f_mode desc, f_word
        )sql");
        db.exec(sql);

        while (db.next()) {
            // LogWriter::write(LogWriterLevel::special, "",
            // db.string("f_word"));
            words.append(db.string("f_word"));
            objects[dbitemscount++] = {db.integer("f_mode"), db.integer("f_id"), db.string("f_orig")};
        }

        totalitems += dbitemscount;
        // FILL PARTNERS MAP
        sql = QString::fromStdString(R"sql(
        select f_id, f_taxname, coalesce(f_taxcode, '') as f_taxcode, f_price_politic,
        f_address, f_permanent_discount,
        f_phone, f_contact, f_name
        from c_partners where f_state>0
        order by 2
        )sql");
        db.exec(sql);
        QJsonArray ja;

        while (db.next()) {
            QJsonObject jt;
            jt["id"] = db.integer("f_id");
            jt["taxname"] = db.string("f_taxname");
            jt["tin"] = db.string("f_taxcode");
            jt["phone"] = db.string("f_phone");
            jt["contact"] = db.string("f_contact");
            jt["address"] = db.string("f_address");
            jt["discount"] = db.doubleValue("f_permanent_discount");
            jt["mode"] = db.integer("f_price_politic");
            jt["name"] = db.string("f_name");
            ja.append(jt);
        }

        mSearchPartners[dbname] = ja;
        // FILL GOODS GROUPS
        sql = QString::fromStdString(R"sql(
        select gr.f_id, gr.f_name, g.f_count
        from c_groups gr
        LEFT JOIN (SELECT g.f_group, COUNT(g.f_id) AS f_count
        FROM c_goods g
        WHERE f_enabled=1
        GROUP BY 1) g ON g.f_group=gr.f_id
        WHERE g.f_count>0
        order by 2
        )sql");
        QJsonArray jgroups;
        db.exec(sql);

        while (db.next()) {
            QJsonObject jt;
            jt["id"] = db.integer("f_id");
            jt["name"] = db.string("f_name");
            jt["count"] = db.integer("f_count");
            jgroups.append(jt);
        }

        mSearchGoodsGroups[dbname] = jgroups;
        // FILL GOODS MAP
        sql = QString::fromStdString(R"sql(
        select g.f_id, gr.f_id as f_groupid, g.f_name, gr.f_name as f_groupname, u.f_name as f_unitname,
        gpr.f_price1, gpr.f_price1disc, gpr.f_price2, gpr.f_price2disc, 0 as f_qty, g.f_scancode
        from c_goods g
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1
        where g.f_enabled=1
        order by gr.f_name, g.f_name
        )sql");
        QJsonArray jgoods;
        db.exec(sql);

        while (db.next()) {
            QJsonObject jt;
            jt["id"] = db.integer("f_id");
            jt["groupid"] = db.integer("f_groupid");
            jt["name"] = db.string("f_name");
            jt["groupname"] = db.string("f_groupname");
            jt["unit"] = db.string("f_unitname");
            jt["p1"] = db.doubleValue("f_price1");
            jt["p1d"] = db.doubleValue("f_price1disc");
            jt["p2"] = db.doubleValue("f_price2");
            jt["p2d"] = db.doubleValue("f_price2disc");
            jt["sku"] = db.string("f_scancode");
            jt["qty"] = 0;
            jgoods.append(jt);
        }

        mSearchGoods[dbname] = jgoods;
    }

    qDebug() << "serch engine of dishes initialized ";
    qDebug() << "total items" << totalitems;
}

void C5SearchEngine::init(const QString &databaseName, const QString &serverKey)
{
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if (!db.open("127.0.0.1", databaseName, "root", "root5", port)) {
        return;
    }

    /* STORAGES */
    QVector<StorageItem> tmp1;
    tmp1.reserve(256);
    db.exec("select f_id, f_name from c_storages order by f_name");

    while (db.next()) {
        QString name = db.string("f_name");
        tmp1.append({db.integer("f_id"), name, name.toLower(), name.toLower().split(" ", Qt::SkipEmptyParts)});
    }

    /* GOODS */
    QVector<GoodsItem> tmp2;
    QHash<int, int> tmplIndex2;
    tmp2.reserve(4096);
    db.exec(QString(mSqlGoods).replace("%where%", ""));

    while (db.next()) {
        QString name = db.string("f_group_name") + " " + db.string("f_name") + " " + db.string("f_scancode");
        GoodsItem g;
        g.id = db.integer("f_id");
        g.groupId = db.integer("f_group_id");
        g.groupName = db.string("f_group_name");
        g.name = db.string("f_name");
        g.unitName = db.string("f_unit_name");
        g.barcode = db.string("f_barcode");
        g.lastInputPrice = db.doubleValue("f_lastinput");
        g.price1 = db.doubleValue("f_price1");
        g.price1disc = db.doubleValue("f_price1disc");
        g.price2 = db.doubleValue("f_price2");
        g.price2disc = db.doubleValue("f_price2disc");
        g.adgt = db.string("f_adgt");
        g.nameLower = name;
        g.words = name.toLower().split(" ", Qt::SkipEmptyParts);
        tmp2.append(g);
        tmplIndex2[tmp2.last().id] = tmp2.size() - 1;
    }

    /* PARTNERS */
    QVector<PartnerItem> tmp3;
    tmp3.reserve(256);
    db.exec(R"(
    select p.f_id, p.f_taxcode, p.f_taxname, p.f_name, p.f_phone
    from c_partners p
    order by p.f_name
    )");

    while (db.next()) {
        QString name = QString("%1 %2 %3 %4")
                           .arg(db.string("f_taxcode"), db.string("f_taxname"), db.string("f_name"), db.string("f_phone"))
                           .toLower();
        tmp3.append({db.integer("f_id"),
                     db.string("f_taxcode"),
                     db.string("f_taxname"),
                     db.string("f_name"),
                     db.string("f_phone"),
                     name,
                     name.split(" ", Qt::SkipEmptyParts)});
    }

    {
        QWriteLocker wl1(&mStoragesLock);
        QWriteLocker wl2(&mGoodsLock);
        QWriteLocker wl3(&mPartnersLock);
        mStorages[serverKey] = std::move(tmp1);
        mGoods[serverKey] = std::move(tmp2);
        mGoodsIndex[serverKey] = tmplIndex2;
        mPartners[serverKey] = std::move(tmp3);
    }

    /* STORE DOC STATUS */
    QVector<StoreDocStatusItem> tmpStoreDocStatus;
    tmpStoreDocStatus.reserve(256);
    db.exec(R"(
    select ss.f_id, ld.f_value as f_name
    from store_statuses ss
    left join l_dictionary ld on ld.f_dict='store_statuses' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
    order by ss.f_id
    )");

    while (db.next()) {
        tmpStoreDocStatus.append({db.integer("f_id"),
                                  db.string("f_name"),
                                  db.string("f_name").toLower(),
                                  db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
    }
    mStoreDocStatus[serverKey] = tmpStoreDocStatus;
    /* STORE DOC TYPE */
    QVector<StoreDocTypeItem> tmpStoreDocTypes;
    tmpStoreDocTypes.reserve(256);
    db.exec(R"(
    select ss.f_id, ld.f_value as f_name
    from store_types ss
    left join l_dictionary ld on ld.f_dict='store_types' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
    order by ss.f_id
    )");

    while (db.next()) {
        tmpStoreDocTypes.append({db.integer("f_id"),
                                 db.string("f_name"),
                                 db.string("f_name").toLower(),
                                 db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
    }
    mStoreDocType[serverKey] = tmpStoreDocTypes;
    /*CURRENCY */
    QVector<StructCurrency> tmpCurrency;
    tmpCurrency.reserve(256);
    db.exec(R"(
    select c.f_id, c.f_short, c.f_name, c.f_symbol, c.f_rate
    from e_currency c
    order by c.f_id
    )");

    while (db.next()) {
        StructCurrency s;
        s.id = db.integer("f_id");
        s.name = db.string("f_name");
        s.nameLower = s.name.toLower();
        s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
        tmpCurrency.append(s);
    }
    mCurrency[serverKey] = tmpCurrency;
    /* cashboxes */
    QVector<StructCashbox> tmpCashbox;
    tmpCashbox.reserve(256);
    db.exec(R"(
    select c.f_id, c.f_name
    from cash_box c
    order by c.f_id
    )");

    while (db.next()) {
        StructCashbox s;
        s.id = db.integer("f_id");
        s.name = db.string("f_name");
        s.nameLower = s.name.toLower();
        s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
        tmpCashbox.append(s);
    }
    mCashbox[serverKey] = tmpCashbox;
    /* PAYMENT TYPES */
    QVector<StructPaymentType> tmpPayment;
    tmpPayment.reserve(256);
    db.exec(R"(
    select pt.f_id, l.f_value as f_name
    from cash_payment_types pt
    inner join l_dictionary l on l.f_dict_id=pt.f_id and l.f_lang='hy'
    where pt.f_dict='cash_payment_types'
    order by pt.f_id
    )");

    while (db.next()) {
        StructPaymentType s;
        s.id = db.integer("f_id");
        s.name = db.string("f_name");
        s.nameLower = s.name.toLower();
        s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
        tmpPayment.append(s);
    }
    mPaymentType[serverKey] = tmpPayment;
    /* EMPLOYEE GROUPS */
    QVector<StructEmployeeGroup> tmpEmployeeGroups;
    tmpEmployeeGroups.reserve(64);
    db.exec(R"(
    SELECT f_id, f_name FROM s_user_group
    )");

    while (db.next()) {
        StructEmployeeGroup s;
        s.id = db.integer("f_id");
        s.name = db.string("f_name");
        s.nameLower = s.name.toLower();
        s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
        tmpEmployeeGroups.append(s);
    }
    mEmployeeGroups[serverKey] = tmpEmployeeGroups;
    /* EMPLOYEES */
    QVector<StructEmployee> tmpEmployee;
    tmpEmployee.reserve(256);
    db.exec(R"(
    SELECT gr.f_name AS f_group_name, u.f_first, u.f_last, u.f_login, u.f_phone, u.f_id
    FROM s_user u
    LEFT JOIN s_user_group gr ON gr.f_id=u.f_id
    WHERE u.f_state=1
    )");

    while (db.next()) {
        StructEmployee s;
        s.id = db.integer("f_id");
        s.groupName = db.string("f_group_name");
        s.firstName = db.string("f_first");
        s.lastName = db.string("f_last");
        s.login = db.string("f_login");
        s.phone = db.string("f_phone");
        s.nameLower = QString("%1 %2 %3 %4 %5")
                          .arg(s.groupName, s.firstName, s.lastName, s.login, s.phone)
                          .toLower();
        s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
        tmpEmployee.append(s);
    }
    mEmployees[serverKey] = tmpEmployee;
    /* TOTALS */
    qDebug() << "storages of" << databaseName << mStorages[serverKey].count() << "items";
    qDebug() << "goods of" << databaseName << mGoods[serverKey].count() << "items";
    qDebug() << "partners of " << databaseName << mPartners[serverKey].count() << "items";
    qDebug() << "store doc types of " << databaseName << mStoreDocType[serverKey].count() << "items";
    qDebug() << "store doc statuses of " << databaseName << mStoreDocStatus[serverKey].count() << "items";
    qDebug() << "currency " << databaseName << mCurrency[serverKey].count() << "items";
    qDebug() << "cashbox " << databaseName << mCashbox[serverKey].count() << "items";
    qDebug() << "payment types " << databaseName << mPaymentType[serverKey].count() << "items";
    qDebug() << "employee groups " << databaseName << mEmployeeGroups[serverKey].count() << "items";
    qDebug() << "employees " << databaseName << mEmployees[serverKey].count() << "items";
}

QString C5SearchEngine::search(const QJsonObject &jo)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    if (jo["template"].toString().isEmpty()) {
        jrep["result_count"] = 0;
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    int maxCount = jo["max_count"].toInt() == 0 ? 10 : jo["max_count"].toInt();
    QString databaseName = jo["database"].toString();
    const QStringList &words = mSearchStrings[databaseName];
    QStringList templateWords = jo["template"].toString().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    templateWords.append(jo["template"].toString());
    QList<int> foundedIndexes;
    bool stopflag = false;

    for (const QString &searchString : std::as_const(templateWords)) {
        for (int i = 0; i < words.count(); i++) {
            if (words.at(i).contains(
                    QRegularExpression("\\b" + QRegularExpression::escape(searchString), QRegularExpression::UseUnicodePropertiesOption))) {
                if (!foundedIndexes.contains(i)) {
                    foundedIndexes.append(i);
                }

                if (foundedIndexes.count() >= maxCount) {
                    stopflag = true;
                    break;
                }
            }

            if (stopflag) {
                break;
            }
        }

        if (stopflag) {
            break;
        }
    }

    qDebug() << "search completed with" << foundedIndexes.count() << "result";
    const QMap<int, SearchObject> &objects = mSearchObjects[databaseName];
    QJsonArray ja;

    for (int i : foundedIndexes) {
        ja.append(QJsonObject{{"mode", objects[i].mode}, {"id", objects[i].objectId}, {"name", objects[i].name}});
    }

    jrep["result_count"] = ja.count();
    jrep["result"] = ja;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}
QString C5SearchEngine::searchPartner(const QJsonObject &jo)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    const QJsonArray &src = mSearchPartners[jo["database"].toString()];
    int page = jo["page"].toInt();
    int limit = jo["limit"].toInt();
    int skip = page * limit;
    QJsonArray result;
    QString searchString = jo["template"].toString();
    bool noResult = true;

    for (int i = 0; i < src.size(); i++) {
        const QJsonObject &jt = src.at(i).toObject();

        if (jt["taxname"].toString().contains(searchString, Qt::CaseInsensitive)
            || jt["tin"].toString().contains(searchString, Qt::CaseInsensitive)
            || jt["phone"].toString().contains(searchString, Qt::CaseInsensitive)
            || jt["contact"].toString().contains(searchString, Qt::CaseInsensitive)
            || jt["address"].toString().contains(searchString, Qt::CaseInsensitive)
            || jt["name"].toString().contains(searchString, Qt::CaseInsensitive)) {
            noResult = false;

            if (skip > 0) {
                skip--;
                continue;
            }

            result.append(jt);

            if (result.size() == limit) {
                break;
            }
        }
    }

    jrep["result_count"] = result.size();
    jrep["result"] = result;
    jrep["page"] = jo["page"];
    jrep["limit"] = jo["limit"];
    jrep["noresult"] = noResult;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchGoodsGroups(const QJsonObject &jo)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    const QJsonArray &src = mSearchGoodsGroups[jo["database"].toString()];
    QJsonArray result;
    QString searchString = jo["template"].toString();

    for (int i = 0; i < src.size(); i++) {
        const QJsonObject &jt = src.at(i).toObject();

        if (jt["name"].toString().contains(searchString, Qt::CaseInsensitive)) {
            result.append(jt);
        }
    }

    jrep["result_count"] = result.size();
    jrep["result"] = result;
    jrep["page"] = jo["page"];
    jrep["limit"] = jo["limit"];
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStore(const QJsonObject &jo)
{
    QString repMsg;
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QJsonArray result;
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if (!db.open("127.0.0.1", jo["database"].toString(), "root", "root5", port)) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchUpdatePartnerCache(const QJsonObject &jo)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString repMsg;
    QString sql = QString::fromStdString(R"sql(
        select f_id, f_taxname, coalesce(f_taxcode, '') as f_taxcode, f_price_politic,
        f_address, f_permanent_discount,
        f_phone, f_contact
        from c_partners where f_state>0 and f_id=:f_id
        order by 2
        )sql");
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if (!db.open("127.0.0.1", jo["database"].toString(), "root", "root5", port)) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    db[":f_id"] = jo["id"].toInt();
    db.exec(sql);
    QJsonArray ja = mSearchPartners[jo["database"].toString()];
    ;
    bool updated = false;

    if (db.next()) {
        QJsonObject jt;
        jt["id"] = db.integer("f_id");
        jt["taxname"] = db.string("f_taxname");
        jt["tin"] = db.string("f_taxcode");
        jt["phone"] = db.string("f_phone");
        jt["contact"] = db.string("f_contact");
        jt["address"] = db.string("f_address");
        jt["discount"] = db.doubleValue("f_permanent_discount");
        jt["mode"] = db.integer("f_price_politic");

        for (int i = 0; i < ja.size(); i++) {
            const QJsonObject &jj = ja.at(i).toObject();

            if (jj["id"].toInt() == jt["id"].toInt()) {
                ja[i] = jt;
                updated = true;
                break;
            }
        }

        if (!updated) {
            ja.append(jt);
        }

        mSearchPartners[jo["database"].toString()] = ja;
        jrep["new"] = !updated;
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStorage(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StorageItem> &siv = mStorages.value(ss.tenantId);
        const int limit = 50000;

        for (const StorageItem &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchGoodsItem(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    // Подготавливаем данные для поиска заранее
    QString barcode = jo.value("barcode").toString().trimmed();
    jrep["barcode"] = barcode;

    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);

    int groupId = jo.contains("group_id") ? jo.value("group_id").toInt() : 0;

    QJsonArray jgoods;

    {
        QReadLocker rl(&mGoodsLock);
        if (!mGoods.contains(ss.tenantId)) {
            jrep["result"] = jgoods;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        const QVector<GoodsItem> &siv = mGoods.value(ss.tenantId);
        const int limit = 50000;

        for (const GoodsItem &si : siv) {
            bool match = false;

            // Сценарий 1: Поиск по штрихкоду
            if (!barcode.isEmpty()) {
                // si.barcode может содержать список через запятую из-за GROUP_CONCAT
                QStringList barcodes = si.barcode.split(',', Qt::SkipEmptyParts);
                for (const QString &b : barcodes) {
                    if (b.trimmed() == barcode) {
                        match = true;
                        break;
                    }
                }

                if (!match) {
                    continue;
                }
            }
            // Сценарий 2: Поиск по группе и названию (если barcode не задан)
            else {
                // Фильтр по группе (если groupId > 0)
                if (groupId > 0 && si.groupId != groupId) {
                    continue;
                }

                // Поиск по словам (каждое слово из запроса должно быть началом какого-то слова в товаре)
                match = true;
                for (const QString &qw : qwords) {
                    bool found = false;
                    for (const QString &w : si.words) {
                        if (w.startsWith(qw, Qt::CaseInsensitive)) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        match = false;
                        break;
                    }
                }

                if (!match) {
                    continue;
                }
            }

            // Если дошли сюда — товар подходит
            jgoods.append(si.toJson());

            // Ограничение выборки или мгновенный выход при нахождении уникального штрихкода
            if (!barcode.isEmpty() || jgoods.size() >= limit) {
                break;
            }
        }
    }

    jrep["result"] = jgoods;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchPartnerItem(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mPartnersLock);
        const QVector<PartnerItem> &siv = mPartners.value(ss.tenantId);
        const int limit = 50000;

        for (const PartnerItem &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(
                {{"f_id", si.id}, {"f_taxcode", si.tin}, {"f_taxname", si.taxName}, {"f_contact", si.contactName}, {"f_phone", si.phone}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::updateDictionary(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString entity = jo.value("entity").toString();

    if (!jo.contains("op")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "What do you want to do with dictionary?";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    QString op = jo.value("op").toString();

    if (op != "i" && op != "u") {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Invalid operation";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    Database db;

    if (!db.open("127.0.0.1", ss.databaseName, "root", "root5", 3306)) {
        LogWriterError(db.lastDbError());
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Database error";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    if (entity == "goods") {
        QWriteLocker wl(&mGoodsLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(QString(mSqlGoods).replace("%where%", "where g.f_id=:f_id"));
        GoodsItem gi;

        if (db.next()) {
            QString name = db.string("f_group_name") + " " + db.string("f_name") + " " + db.string("f_scancode");
            gi.id = db.integer("f_id");
            gi.groupName = db.string("f_group_name");
            gi.name = db.string("f_name");
            gi.barcode = db.string("f_scancode");
            gi.unitName = db.string("f_unit_name");
            gi.nameLower = name.toLower();
            gi.words = name.toLower().split(" ", Qt::SkipEmptyParts);
        } else {
            QString err = QString("Invalid record with id=%1").arg(jo.value("id").toInt());
            LogWriterError(err);
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = err;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<GoodsItem> &siv = mGoods[ss.tenantId];
        QHash<int, int> &sin = mGoodsIndex[ss.tenantId];

        if (op == "i") {
            if (sin.contains(gi.id)) {
                siv[sin.value(gi.id)] = gi;
            } else {
                siv.append(gi);
                sin[gi.id] = siv.size() - 1;
            }
        } else {
            auto it = sin.find(gi.id);

            if (it == sin.end()) {
                siv.append(gi);
                sin[gi.id] = siv.size() - 1;
            } else {
                siv[*it] = gi;
            }
        }
    } else if (entity == "employee") {
        QWriteLocker wl(&mEmployeeLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(R"(
        SELECT gr.f_name AS f_group_name, u.f_first, u.f_last, u.f_login, u.f_phone, u.f_id
        FROM s_user u
        LEFT JOIN s_user_group gr ON gr.f_id=u.f_id
        WHERE u.f_state=1 and u.f_id=:f_id
        )");
        StructEmployee se;

        if (db.next()) {
            se.id = db.integer("f_id");
            se.groupName = db.string("f_group_name");
            se.firstName = db.string("f_first");
            se.lastName = db.string("f_last");
            se.login = db.string("f_login");
            se.phone = db.string("f_phone");
            se.nameLower = QString("%1 %2 %3 %4 %5")
                               .arg(se.groupName, se.firstName, se.lastName, se.login, se.phone)
                               .toLower();
            se.words = se.nameLower.split(" ", Qt::SkipEmptyParts);
        } else {
            QString err = QString("Invalid employee with id=%1").arg(jo.value("id").toInt());
            LogWriterError(err);
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = err;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<StructEmployee> &siv = mEmployees[ss.tenantId];

        auto updateOrInsert = [&siv](const StructEmployee &e) {
            for (int i = 0; i < siv.size(); i++) {
                if (siv[i].id == e.id) {
                    siv[i] = e;
                    return true;
                }
            }

            siv.append(e);
            return false;
        };

        if (op == "i") {
            updateOrInsert(se);
        } else {
            updateOrInsert(se);
        }
    } else if (entity == "employee_group") {
        QWriteLocker wl(&mEmployeeGroupLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(R"(
        SELECT f_id, f_name FROM s_user_group
        WHERE f_id=:f_id
        )");
        StructEmployeeGroup sg;

        if (db.next()) {
            sg.id = db.integer("f_id");
            sg.name = db.string("f_name");
            sg.nameLower = sg.name.toLower();
            sg.words = sg.nameLower.split(" ", Qt::SkipEmptyParts);
        } else {
            QString err = QString("Invalid employee group with id=%1").arg(jo.value("id").toInt());
            LogWriterError(err);
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = err;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<StructEmployeeGroup> &siv = mEmployeeGroups[ss.tenantId];
        bool updated = false;

        for (int i = 0; i < siv.size(); i++) {
            if (siv[i].id == sg.id) {
                siv[i] = sg;
                updated = true;
                break;
            }
        }

        if (!updated) {
            siv.append(sg);
        }
    }

    jrep["status"] = 1;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStoreDocStatus(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StoreDocStatusItem> &siv = mStoreDocStatus.value(ss.tenantId);
        const int limit = 50000;

        for (const StoreDocStatusItem &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStoreDocType(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StoreDocTypeItem> &siv = mStoreDocType.value(ss.tenantId);
        const int limit = 50000;

        for (const StoreDocTypeItem &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchCurrency(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mCurrencyLock);
        const QVector<StructCurrency> &siv = mCurrency.value(ss.tenantId);
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchCashbox(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mCashboxLock);
        const QVector<StructCashbox> &siv = mCashbox.value(ss.tenantId);
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchPaymentType(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mPaymentTypeLock);
        const QVector<StructPaymentType> &siv = mPaymentType.value(ss.tenantId);
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jstorages.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jstorages.size() >= limit)
                break;
        }
    }
    jrep["result"] = jstorages;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchEmployee(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jemployees;
    {
        QReadLocker rl(&mEmployeeLock);
        const QVector<StructEmployee> &siv = mEmployees.value(ss.tenantId);
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jemployees.append(QJsonObject{{"f_id", si.id},
                                          {"f_group_name", si.groupName},
                                          {"f_first", si.firstName},
                                          {"f_last", si.lastName},
                                          {"f_login", si.login},
                                          {"f_phone", si.phone}});

            if (jemployees.size() >= limit)
                break;
        }
    }
    jrep["result"] = jemployees;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchEmployeeGroup(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jgroups;
    {
        QReadLocker rl(&mEmployeeGroupLock);
        const QVector<StructEmployeeGroup> &siv = mEmployeeGroups.value(ss.tenantId);
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

            for (const QString &qw : qwords) {
                bool found = false;

                for (const QString &w : si.words) {
                    if (w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            jgroups.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jgroups.size() >= limit)
                break;
        }
    }
    jrep["result"] = jgroups;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchUpdateGoodsLastInputPrices(const QJsonObject &jo, const SocketStruct &ss)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    // Данные приходят в поле "data", которое содержит JSON-строку с массивом товаров
    // Согласно PHP коду: $notify("update_goods_last_input_prices", json_encode($items, ...), $isnew);
    QString dataStr = jo.value("data").toString();
    QJsonDocument doc = QJsonDocument::fromJson(dataStr.toUtf8());

    if (!doc.isArray()) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Invalid data format: expected array of items";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    QJsonArray items = doc.array();
    int updatedCount = 0;

    {
        // Блокируем кэш товаров на запись
        QWriteLocker locker(&mGoodsLock);

        if (!mGoods.contains(ss.tenantId)) {
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = "Tenant cache not found";
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<GoodsItem> &goodsVector = mGoods[ss.tenantId];
        QHash<int, int> &indexMap = mGoodsIndex[ss.tenantId];

        for (int i = 0; i < items.size(); ++i) {
            QJsonObject itemObj = items.at(i).toObject();
            int itemId = itemObj.value("item_id").toInt();
            double lastPrice = itemObj.value("price").toDouble();

            // Ищем товар в кэше по ID через индексную карту
            if (indexMap.contains(itemId)) {
                int vectorIndex = indexMap.value(itemId);
                if (vectorIndex >= 0 && vectorIndex < goodsVector.size()) {
                    goodsVector[vectorIndex].lastInputPrice = lastPrice;
                    updatedCount++;
                }
            }
        }
    }

    qDebug() << "Updated last input prices for" << updatedCount << "items in database:" << ss.databaseName;

    jrep["updated_count"] = updatedCount;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}
