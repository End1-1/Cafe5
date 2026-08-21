#include "c5searchengine.h"
#include <QElapsedTimer>
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
#include "struct_goods_type.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_payment_type.h"
#include "struct_storage_item.h"
#include "struct_goods_group.h"

namespace {

QString digitsOnly(const QString &s)
{
    QString out;
    out.reserve(s.size());
    for (const QChar &c : s) {
        if (c.isDigit()) {
            out.append(c);
        }
    }
    return out;
}

PartnerItem makePartnerItem(int id, const QString &tin, const QString &taxName, const QString &name, const QString &phone)
{
    const QString phoneDigits = digitsOnly(phone);
    QString searchable = QStringLiteral("%1 %2 %3 %4")
                             .arg(tin, taxName, name, phone)
                             .toLower();
    if (!phoneDigits.isEmpty()) {
        searchable += QLatin1Char(' ');
        searchable += phoneDigits;
    }
    PartnerItem p;
    p.id = id;
    p.tin = tin;
    p.taxName = taxName;
    p.contactName = name;
    p.phone = phone;
    p.nameLower = searchable;
    p.words = searchable.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    return p;
}

bool partnerMatchesQuery(const PartnerItem &si, const QStringList &qwords)
{
    if (qwords.isEmpty()) {
        return true;
    }
    const QString phoneDigits = digitsOnly(si.phone);
    for (const QString &qw : qwords) {
        bool found = false;
        if (si.nameLower.contains(qw)) {
            found = true;
        } else {
            for (const QString &w : si.words) {
                if (w.startsWith(qw)) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            const QString qwDigits = digitsOnly(qw);
            if (!qwDigits.isEmpty() && phoneDigits.contains(qwDigits)) {
                found = true;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

} // namespace

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
    g.f_lastinputprice AS f_lastinput,
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
QReadWriteLock mGoodsTypeLock;
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
QHash<QString, QVector<StructGoodsType>> mGoodsType;
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

namespace
{

constexpr int kSeLogTemplateMax = 80;
constexpr const char *kSeLogTitle = "C5SearchEngine";

QString seLogTruncate(const QString &s)
{
    const QString t = s.trimmed();

    if(t.size() <= kSeLogTemplateMax) {
        return t;
    }

    return t.left(kSeLogTemplateMax - 3) + QStringLiteral("...");
}

void seLogVerbose(const QString &message)
{
    LogWriterVerbose(kSeLogTitle, message);
}

void seLogWarning(const QString &message)
{
    LogWriter::write(LogWriterLevel::warning, kSeLogTitle, message);
}

void seLogSearchResult(const char *method,
                       const QString &key,
                       const QString &query,
                       int cacheSize,
                       int found,
                       qint64 elapsedMs,
                       const QString &extra = QString())
{
    QString msg = QStringLiteral("%1 key=%2 template=\"%3\" cache=%4 found=%5 ms=%6")
                      .arg(QLatin1String(method), key, seLogTruncate(query))
                      .arg(cacheSize)
                      .arg(found)
                      .arg(elapsedMs);

    if(!extra.isEmpty()) {
        msg += QLatin1Char(' ');
        msg += extra;
    }

    seLogVerbose(msg);
}

void seLogEmptyCache(const char *method, const QString &key)
{
    seLogWarning(QStringLiteral("%1 empty cache key=%2").arg(QLatin1String(method), key));
}

int seDictionaryCount(const QString &engine, const QString &serverKey)
{
    if(engine == SelectorName<StorageItem>::value) {
        return mStorages.value(serverKey).size();
    }

    if(engine == SelectorName<GoodsItem>::value) {
        return mGoods.value(serverKey).size();
    }

    if(engine == SelectorName<PartnerItem>::value) {
        return mPartners.value(serverKey).size();
    }

    if(engine == SelectorName<StoreDocStatusItem>::value) {
        return mStoreDocStatus.value(serverKey).size();
    }

    if(engine == SelectorName<StoreDocTypeItem>::value) {
        return mStoreDocType.value(serverKey).size();
    }

    if(engine == SelectorName<StructCurrency>::value) {
        return mCurrency.value(serverKey).size();
    }

    if(engine == SelectorName<StructCashbox>::value) {
        return mCashbox.value(serverKey).size();
    }

    if(engine == SelectorName<StructPaymentType>::value) {
        return mPaymentType.value(serverKey).size();
    }

    if(engine == SelectorName<StructGoodsType>::value) {
        return mGoodsType.value(serverKey).size();
    }

    if(engine == SelectorName<StructEmployeeGroup>::value) {
        return mEmployeeGroups.value(serverKey).size();
    }

    if(engine == SelectorName<StructEmployee>::value) {
        return mEmployees.value(serverKey).size();
    }

    if(engine == SelectorName<GoodsGroupItem>::value) {
        return mSearchGoodsGroups.value(serverKey).count();
    }

    return -1;
}

} // namespace

static QJsonArray loadGoodsGroupsFromDb(Database &db)
{
    QJsonArray jgroups;
    if (!db.exec(R"sql(
        SELECT
            gr.f_id,
            gr.f_parent,
            gr.f_name,
            CAST(gr.f_class AS SIGNED) AS f_class,
            COALESCE(gr.f_color, 0) AS f_color,
            COALESCE(gr.f_order, 0) AS f_order,
            COALESCE(g.f_count, 0) AS f_count
        FROM c_groups gr
        LEFT JOIN (
            SELECT g.f_group, COUNT(g.f_id) AS f_count
            FROM c_goods g
            WHERE g.f_enabled = 1
            GROUP BY 1
        ) g ON g.f_group = gr.f_id
        ORDER BY gr.f_name
    )sql")) {
        LogWriterError(QString("C5SearchEngine loadGoodsGroupsFromDb SQL failed err=%1")
                           .arg(db.lastDbError()));
        return jgroups;
    }

    while (db.next()) {
        QJsonObject jt;
        jt["f_id"] = db.integer("f_id");
        jt["f_parent"] = db.integer("f_parent");
        jt["f_name"] = db.string("f_name");
        jt["f_class"] = db.integer("f_class");
        jt["f_color"] = db.integer("f_color");
        jt["f_order"] = db.integer("f_order");
        jt["f_queue"] = db.integer("f_order");
        jt["count"] = db.integer("f_count");
        jgroups.append(jt);
    }

    return jgroups;
}

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
    mSearchGoods.clear();

    seLogVerbose(QStringLiteral("init(list) start databases=%1").arg(databases.join(QLatin1Char(','))));

    for (const QString &dbname : databases) {
        int dbitemscount = 0;
        int partnersCount = 0;
        int goodsCount = 0;
        int port = 3306;
#ifdef QT_DEBUG
        port = 3306;
#endif

        if (!db.open("127.0.0.1", dbname, "root", "root5", port)) {
            LogWriterError(QStringLiteral("C5SearchEngine::init(list) open failed db=%1 err=%2")
                               .arg(dbname, db.lastDbError()));
            continue;
        }

        seLogVerbose(QStringLiteral("init(list) loading db=%1").arg(dbname));

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
        partnersCount = ja.size();
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
        goodsCount = jgoods.size();

        seLogVerbose(QStringLiteral("init(list) db=%1 dishes=%2 partners=%3 goods=%4")
                         .arg(dbname)
                         .arg(dbitemscount)
                         .arg(partnersCount)
                         .arg(goodsCount));
    }

    seLogVerbose(QStringLiteral("init(list) done total_dishes=%1").arg(totalitems));
}

void C5SearchEngine::init(const QString &databaseName, const QString &serverKey)
{
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    LogWriterVerbose("C5SearchEngine::init",
                     QString("start db=%1 key=%2 host=127.0.0.1 port=%3")
                         .arg(databaseName, serverKey)
                         .arg(port));

    if (!db.open("127.0.0.1", databaseName, "root", "root5", port)) {
        LogWriterError(QString("C5SearchEngine::init open failed db=%1 key=%2 err=%3")
                           .arg(databaseName, serverKey, db.lastDbError()));
        return;
    }

    auto execOrLog = [&](const char *block, const QString &sql) -> bool {
        if (db.exec(sql)) {
            return true;
        }
        LogWriterError(QString("C5SearchEngine::init [%1] SQL failed db=%2 key=%3 err=%4")
                           .arg(QLatin1String(block), databaseName, serverKey, db.lastDbError()));
        return false;
    };

    auto logLoaded = [&](const char *block, int count) {
        LogWriterVerbose("C5SearchEngine::init",
                         QString("[%1] loaded %2 items (db=%3 key=%4)")
                             .arg(QLatin1String(block))
                             .arg(count)
                             .arg(databaseName, serverKey));
    };

    /* STORAGES */
    QVector<StorageItem> tmp1;
    tmp1.reserve(256);
    if (execOrLog("storages", "select f_id, f_name from c_storages order by f_name")) {
        while (db.next()) {
            QString name = db.string("f_name");
            tmp1.append({db.integer("f_id"), name, name.toLower(), name.toLower().split(" ", Qt::SkipEmptyParts)});
        }
    }
    logLoaded("storages", tmp1.size());

    /* GOODS */
    QVector<GoodsItem> tmp2;
    QHash<int, int> tmplIndex2;
    tmp2.reserve(4096);
    if (execOrLog("goods", QString(mSqlGoods).replace("%where%", " where g.f_enabled=1 "))) {
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
    }
    logLoaded("goods", tmp2.size());

    /* PARTNERS */
    QVector<PartnerItem> tmp3;
    tmp3.reserve(256);
    if (execOrLog("partners", R"(
    select p.f_id, p.f_taxcode, p.f_taxname, p.f_name, p.f_phone
    from c_partners p
    order by p.f_name
    )")) {
        while (db.next()) {
            tmp3.append(makePartnerItem(db.integer("f_id"),
                                        db.string("f_taxcode"),
                                        db.string("f_taxname"),
                                        db.string("f_name"),
                                        db.string("f_phone")));
        }
    }
    logLoaded("partners", tmp3.size());

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
    if (execOrLog("store_statuses", R"(
    select ss.f_id, ld.f_value as f_name
    from store_statuses ss
    left join l_dictionary ld on ld.f_dict='store_statuses' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
    order by ss.f_id
    )")) {
        while (db.next()) {
            tmpStoreDocStatus.append({db.integer("f_id"),
                                      db.string("f_name"),
                                      db.string("f_name").toLower(),
                                      db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
        }
    }
    mStoreDocStatus[serverKey] = tmpStoreDocStatus;
    logLoaded("store_statuses", tmpStoreDocStatus.size());

    /* STORE DOC TYPE */
    QVector<StoreDocTypeItem> tmpStoreDocTypes;
    tmpStoreDocTypes.reserve(256);
    if (execOrLog("store_types", R"(
    select ss.f_id, ld.f_value as f_name
    from store_types ss
    left join l_dictionary ld on ld.f_dict='store_types' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
    order by ss.f_id
    )")) {
        while (db.next()) {
            tmpStoreDocTypes.append({db.integer("f_id"),
                                     db.string("f_name"),
                                     db.string("f_name").toLower(),
                                     db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
        }
    }
    mStoreDocType[serverKey] = tmpStoreDocTypes;
    logLoaded("store_types", tmpStoreDocTypes.size());

    /*CURRENCY */
    QVector<StructCurrency> tmpCurrency;
    tmpCurrency.reserve(256);
    if (execOrLog("currency", R"(
    select c.f_id, c.f_short, c.f_name, c.f_symbol, c.f_rate
    from e_currency c
    order by c.f_id
    )")) {
        while (db.next()) {
            StructCurrency s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmpCurrency.append(s);
        }
    }
    mCurrency[serverKey] = tmpCurrency;
    logLoaded("currency", tmpCurrency.size());

    /* cashboxes */
    QVector<StructCashbox> tmpCashbox;
    tmpCashbox.reserve(256);
    if (execOrLog("cashbox", R"(
    select c.f_id, c.f_name
    from cash_box c
    order by c.f_id
    )")) {
        while (db.next()) {
            StructCashbox s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmpCashbox.append(s);
        }
    }
    mCashbox[serverKey] = tmpCashbox;
    logLoaded("cashbox", tmpCashbox.size());

    /* PAYMENT TYPES */
    QVector<StructPaymentType> tmpPayment;
    tmpPayment.reserve(256);
    if (execOrLog("payment_types", R"(
    select pt.f_id, l.f_value as f_name
    from cash_payment_types pt
    inner join l_dictionary l on l.f_dict_id=pt.f_id and l.f_lang='hy'
    where l.f_dict='cash_payment_types'
    order by pt.f_id
    )")) {
        while (db.next()) {
            StructPaymentType s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmpPayment.append(s);
        }
    }
    mPaymentType[serverKey] = tmpPayment;
    logLoaded("payment_types", tmpPayment.size());

    /* GOODS TYPES (LEFT JOIN: rows in c_goods_type without l_dictionary still searchable) */
    QVector<StructGoodsType> tmpGoodsType;
    tmpGoodsType.reserve(256);
    if (execOrLog("goods_types", R"(
    SELECT gt.f_id,
           COALESCE(NULLIF(TRIM(l.f_value), ''), CAST(gt.f_id AS CHAR)) AS f_type_name
    FROM c_goods_type gt
    LEFT JOIN l_dictionary l ON l.f_dict='c_goods_type' AND l.f_lang='hy' AND l.f_dict_id=gt.f_id
    ORDER BY gt.f_id
    )")) {
        while (db.next()) {
            StructGoodsType s;
            s.id = db.integer("f_id");
            s.name = db.string("f_type_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmpGoodsType.append(s);
        }
    }
    mGoodsType[serverKey] = tmpGoodsType;
    logLoaded("goods_types", tmpGoodsType.size());

    /* EMPLOYEE GROUPS */
    QVector<StructEmployeeGroup> tmpEmployeeGroups;
    tmpEmployeeGroups.reserve(64);
    if (execOrLog("employee_groups", R"(
    SELECT f_id, f_name FROM s_user_group
    )")) {
        while (db.next()) {
            StructEmployeeGroup s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmpEmployeeGroups.append(s);
        }
    }
    mEmployeeGroups[serverKey] = tmpEmployeeGroups;
    logLoaded("employee_groups", tmpEmployeeGroups.size());

    /* EMPLOYEES */
    QVector<StructEmployee> tmpEmployee;
    tmpEmployee.reserve(256);
    if (execOrLog("employees", R"(
    SELECT gr.f_name AS f_group_name, u.f_group, u.f_first, u.f_last, u.f_login, u.f_phone, u.f_id
    FROM s_user u
    LEFT JOIN s_user_group gr ON gr.f_id=u.f_group
    WHERE u.f_state=1
    )")) {
        while (db.next()) {
            StructEmployee s;
            s.id = db.integer("f_id");
            s.groupId = db.integer("f_group");
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
    }
    mEmployees[serverKey] = tmpEmployee;
    logLoaded("employees", tmpEmployee.size());

    /* GOODS GROUPS */
    mSearchGoodsGroups[serverKey] = loadGoodsGroupsFromDb(db);
    logLoaded("goods_groups", mSearchGoodsGroups[serverKey].count());

    const int storagesCount = mStorages.value(serverKey).count();
    const int goodsCount = mGoods.value(serverKey).count();
    const int partnersCount = mPartners.value(serverKey).count();
    const int storeTypesCount = mStoreDocType.value(serverKey).count();
    const int storeStatusesCount = mStoreDocStatus.value(serverKey).count();
    const int currencyCount = mCurrency.value(serverKey).count();
    const int cashboxCount = mCashbox.value(serverKey).count();
    const int paymentTypesCount = mPaymentType.value(serverKey).count();
    const int goodsTypesCount = mGoodsType.value(serverKey).count();
    const int employeeGroupsCount = mEmployeeGroups.value(serverKey).count();
    const int employeesCount = mEmployees.value(serverKey).count();
    const int goodsGroupsCount = mSearchGoodsGroups.value(serverKey).count();

    LogWriterVerbose(
        "C5SearchEngine::init",
        QString("done db=%1 key=%2").arg(databaseName, serverKey)
            + QString(" storages=%1").arg(storagesCount)
            + QString(" goods=%1").arg(goodsCount)
            + QString(" partners=%1").arg(partnersCount)
            + QString(" store_types=%1").arg(storeTypesCount)
            + QString(" store_statuses=%1").arg(storeStatusesCount)
            + QString(" currency=%1").arg(currencyCount)
            + QString(" cashbox=%1").arg(cashboxCount)
            + QString(" payment_types=%1").arg(paymentTypesCount)
            + QString(" goods_types=%1").arg(goodsTypesCount)
            + QString(" employee_groups=%1").arg(employeeGroupsCount)
            + QString(" employees=%1").arg(employeesCount)
            + QString(" goods_groups=%1").arg(goodsGroupsCount));

    if (storagesCount == 0 && goodsCount == 0 && partnersCount == 0 && storeTypesCount == 0
        && storeStatusesCount == 0 && currencyCount == 0 && cashboxCount == 0 && paymentTypesCount == 0
        && goodsTypesCount == 0 && employeeGroupsCount == 0 && employeesCount == 0
        && goodsGroupsCount == 0) {
        LogWriter::write(LogWriterLevel::warning,
                         "C5SearchEngine::init",
                         QString("all dictionaries empty after load db=%1 key=%2")
                             .arg(databaseName, serverKey));
    }
}

QString C5SearchEngine::reloadDictionary(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];

    const QString engine = jo.value("engine").toString();
    if (engine.isEmpty()) {
        seLogWarning(QStringLiteral("reloadDictionary engine not specified key=%1 db=%2")
                         .arg(ss.tenantId, ss.databaseName));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Dictionary engine is not specified";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    seLogVerbose(QStringLiteral("reloadDictionary start engine=%1 key=%2 db=%3")
                     .arg(engine, ss.tenantId, ss.databaseName));

    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if (!db.open("127.0.0.1", ss.databaseName, "root", "root5", port)) {
        LogWriterError(QStringLiteral("C5SearchEngine::reloadDictionary open failed key=%1 db=%2 err=%3")
                           .arg(ss.tenantId, ss.databaseName, db.lastDbError()));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Database error";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    const QString &serverKey = ss.tenantId;

    if (engine == SelectorName<StorageItem>::value) {
        QWriteLocker wl(&mStoragesLock);
        QVector<StorageItem> tmp;
        tmp.reserve(256);
        db.exec("select f_id, f_name from c_storages order by f_name");
        while (db.next()) {
            QString name = db.string("f_name");
            tmp.append({db.integer("f_id"), name, name.toLower(), name.toLower().split(" ", Qt::SkipEmptyParts)});
        }
        mStorages[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<GoodsItem>::value) {
        QWriteLocker wl(&mGoodsLock);
        QVector<GoodsItem> tmp;
        QHash<int, int> tmpIndex;
        tmp.reserve(4096);
        db.exec(QString(mSqlGoods).replace("%where%", " where g.f_enabled=1 "));
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
            tmp.append(g);
            tmpIndex[tmp.last().id] = tmp.size() - 1;
        }
        mGoods[serverKey] = std::move(tmp);
        mGoodsIndex[serverKey] = std::move(tmpIndex);
    } else if (engine == SelectorName<PartnerItem>::value) {
        QWriteLocker wl(&mPartnersLock);
        QVector<PartnerItem> tmp;
        tmp.reserve(256);
        db.exec(R"(
        select p.f_id, p.f_taxcode, p.f_taxname, p.f_name, p.f_phone
        from c_partners p
        order by p.f_name
        )");
        while (db.next()) {
            tmp.append(makePartnerItem(db.integer("f_id"),
                                       db.string("f_taxcode"),
                                       db.string("f_taxname"),
                                       db.string("f_name"),
                                       db.string("f_phone")));
        }
        mPartners[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StoreDocStatusItem>::value) {
        QVector<StoreDocStatusItem> tmp;
        tmp.reserve(256);
        db.exec(R"(
        select ss.f_id, ld.f_value as f_name
        from store_statuses ss
        left join l_dictionary ld on ld.f_dict='store_statuses' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
        order by ss.f_id
        )");
        while (db.next()) {
            tmp.append({db.integer("f_id"),
                        db.string("f_name"),
                        db.string("f_name").toLower(),
                        db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
        }
        mStoreDocStatus[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StoreDocTypeItem>::value) {
        QVector<StoreDocTypeItem> tmp;
        tmp.reserve(256);
        db.exec(R"(
        select ss.f_id, ld.f_value as f_name
        from store_types ss
        left join l_dictionary ld on ld.f_dict='store_types' and ld.f_dict_id=ss.f_id and ld.f_lang='hy'
        order by ss.f_id
        )");
        while (db.next()) {
            tmp.append({db.integer("f_id"),
                        db.string("f_name"),
                        db.string("f_name").toLower(),
                        db.string("f_name").toLower().split(" ", Qt::SkipEmptyParts)});
        }
        mStoreDocType[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructCurrency>::value) {
        QWriteLocker wl(&mCurrencyLock);
        QVector<StructCurrency> tmp;
        tmp.reserve(256);
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
            tmp.append(s);
        }
        mCurrency[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructCashbox>::value) {
        QWriteLocker wl(&mCashboxLock);
        QVector<StructCashbox> tmp;
        tmp.reserve(256);
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
            tmp.append(s);
        }
        mCashbox[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructPaymentType>::value) {
        QWriteLocker wl(&mPaymentTypeLock);
        QVector<StructPaymentType> tmp;
        tmp.reserve(256);
        db.exec(R"(
        select pt.f_id, l.f_value as f_name
        from cash_payment_types pt
        inner join l_dictionary l on l.f_dict_id=pt.f_id and l.f_lang='hy'
        where l.f_dict='cash_payment_types'
        order by pt.f_id
        )");
        while (db.next()) {
            StructPaymentType s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmp.append(s);
        }
        mPaymentType[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructGoodsType>::value) {
        QWriteLocker wl(&mGoodsTypeLock);
        QVector<StructGoodsType> tmp;
        tmp.reserve(256);
        db.exec(R"(
        SELECT gt.f_id,
               COALESCE(NULLIF(TRIM(l.f_value), ''), CAST(gt.f_id AS CHAR)) AS f_type_name
        FROM c_goods_type gt
        LEFT JOIN l_dictionary l ON l.f_dict='c_goods_type' AND l.f_lang='hy' AND l.f_dict_id=gt.f_id
        ORDER BY gt.f_id
        )");
        while (db.next()) {
            StructGoodsType s;
            s.id = db.integer("f_id");
            s.name = db.string("f_type_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmp.append(s);
        }
        mGoodsType[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructEmployeeGroup>::value) {
        QWriteLocker wl(&mEmployeeGroupLock);
        QVector<StructEmployeeGroup> tmp;
        tmp.reserve(64);
        db.exec(R"(SELECT f_id, f_name FROM s_user_group)");
        while (db.next()) {
            StructEmployeeGroup s;
            s.id = db.integer("f_id");
            s.name = db.string("f_name");
            s.nameLower = s.name.toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmp.append(s);
        }
        mEmployeeGroups[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<StructEmployee>::value) {
        QWriteLocker wl(&mEmployeeLock);
        QVector<StructEmployee> tmp;
        tmp.reserve(256);
        db.exec(R"(
        SELECT gr.f_name AS f_group_name, u.f_group, u.f_first, u.f_last, u.f_login, u.f_phone, u.f_id
        FROM s_user u
        LEFT JOIN s_user_group gr ON gr.f_id=u.f_group
        WHERE u.f_state=1
        )");
        while (db.next()) {
            StructEmployee s;
            s.id = db.integer("f_id");
            s.groupId = db.integer("f_group");
            s.groupName = db.string("f_group_name");
            s.firstName = db.string("f_first");
            s.lastName = db.string("f_last");
            s.login = db.string("f_login");
            s.phone = db.string("f_phone");
            s.nameLower = QString("%1 %2 %3 %4 %5")
                              .arg(s.groupName, s.firstName, s.lastName, s.login, s.phone)
                              .toLower();
            s.words = s.nameLower.split(" ", Qt::SkipEmptyParts);
            tmp.append(s);
        }
        mEmployees[serverKey] = std::move(tmp);
    } else if (engine == SelectorName<GoodsGroupItem>::value) {
        mSearchGoodsGroups[serverKey] = loadGoodsGroupsFromDb(db);
    } else {
        LogWriterError(QStringLiteral("C5SearchEngine::reloadDictionary unknown engine=%1 key=%2")
                           .arg(engine, serverKey));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = QString("Unknown dictionary engine: %1").arg(engine);
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    const int loaded = seDictionaryCount(engine, serverKey);
    seLogVerbose(QStringLiteral("reloadDictionary done engine=%1 key=%2 loaded=%3 ms=%4")
                     .arg(engine, serverKey)
                     .arg(loaded)
                     .arg(timer.elapsed()));

    if(loaded == 0) {
        seLogWarning(QStringLiteral("reloadDictionary empty cache engine=%1 key=%2 db=%3")
                         .arg(engine, serverKey, ss.databaseName));
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::search(const QJsonObject &jo)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    const QString databaseName = jo["database"].toString();
    const QString templateText = jo["template"].toString();

    if (templateText.isEmpty()) {
        seLogVerbose(QStringLiteral("search empty template db=%1").arg(databaseName));
        jrep["result_count"] = 0;
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    int maxCount = jo["max_count"].toInt() == 0 ? 10 : jo["max_count"].toInt();
    const QStringList &words = mSearchStrings[databaseName];
    QStringList templateWords = templateText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    templateWords.append(templateText);
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

    const QMap<int, SearchObject> &objects = mSearchObjects[databaseName];
    QJsonArray ja;

    for (int i : foundedIndexes) {
        ja.append(QJsonObject{{"mode", objects[i].mode}, {"id", objects[i].objectId}, {"name", objects[i].name}});
    }

    jrep["result_count"] = ja.count();
    jrep["result"] = ja;

    seLogSearchResult("search", databaseName, templateText, words.count(), ja.count(), timer.elapsed());

    if(words.isEmpty()) {
        seLogEmptyCache("search", databaseName);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}
QString C5SearchEngine::searchPartner(const QJsonObject &jo)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    const QString databaseName = jo["database"].toString();
    const QJsonArray &src = mSearchPartners[databaseName];
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

    seLogSearchResult("searchPartner",
                      databaseName,
                      searchString,
                      src.size(),
                      result.size(),
                      timer.elapsed(),
                      QStringLiteral("page=%1 limit=%2").arg(page).arg(limit));

    if(src.isEmpty()) {
        seLogEmptyCache("searchPartner", databaseName);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchGoodsGroups(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    // Cache is keyed by tenantId (uuid.dbname), not by short database name from client.
    const QString dbKey = ss.tenantId;
    const QJsonArray &src = mSearchGoodsGroups.value(dbKey);
    QString needle = jo["lower_name"].toString().trimmed();
    if (needle.isEmpty()) {
        needle = jo["template"].toString().trimmed().toLower();
    }
    const QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray result;
    const int limit = jo["limit"].toInt() > 0 ? jo["limit"].toInt() : 50000;

    for (int i = 0; i < src.size(); ++i) {
        const QJsonObject jt = src.at(i).toObject();
        const QString name = jt["f_name"].toString();
        const QString nameLower = name.toLower();

        bool match = true;
        for (const QString &qw : qwords) {
            if (!nameLower.contains(qw)) {
                match = false;
                break;
            }
        }

        if (!match) {
            continue;
        }

        result.append(jt);
        if (result.size() >= limit) {
            break;
        }
    }

    jrep["result_count"] = result.size();
    jrep["result"] = result;
    jrep["page"] = jo["page"];
    jrep["limit"] = jo["limit"];

    seLogSearchResult("searchGoodsGroups", dbKey, needle, src.size(), result.size(), timer.elapsed());

    if(src.isEmpty()) {
        seLogEmptyCache("searchGoodsGroups", dbKey);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStore(const QJsonObject &jo)
{
    QElapsedTimer timer;
    timer.start();

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
        LogWriterError(QStringLiteral("C5SearchEngine::searchStore open failed db=%1 err=%2")
                           .arg(jo["database"].toString(), db.lastDbError()));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    seLogSearchResult("searchStore", jo["database"].toString(), QString(), 0, 0, timer.elapsed());

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchUpdatePartnerCache(const QJsonObject &jo)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    const QString databaseName = jo["database"].toString();
    QString sql = QString::fromStdString(R"sql(
        select f_id, f_taxname, coalesce(f_taxcode, '') as f_taxcode, f_price_politic,
        f_address, f_permanent_discount,
        f_phone, f_contact, f_name
        from c_partners where f_state>0 and f_id=:f_id
        order by 2
        )sql");
    Database db;
    int port = 3306;
#ifdef QT_DEBUG
    port = 3306;
#endif

    if (!db.open("127.0.0.1", databaseName, "root", "root5", port)) {
        LogWriterError(QStringLiteral("C5SearchEngine::searchUpdatePartnerCache open failed db=%1 err=%2")
                           .arg(databaseName, db.lastDbError()));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    db[":f_id"] = jo["id"].toInt();
    db.exec(sql);
    QJsonArray ja = mSearchPartners[databaseName];
    bool updated = false;
    PartnerItem pi;

    if (db.next()) {
        pi = makePartnerItem(db.integer("f_id"),
                             db.string("f_taxcode"),
                             db.string("f_taxname"),
                             db.string("f_name"),
                             db.string("f_phone"));

        QJsonObject jt;
        jt["id"] = pi.id;
        jt["taxname"] = pi.taxName;
        jt["tin"] = pi.tin;
        jt["phone"] = pi.phone;
        jt["contact"] = db.string("f_contact");
        jt["address"] = db.string("f_address");
        jt["discount"] = db.doubleValue("f_permanent_discount");
        jt["mode"] = db.integer("f_price_politic");
        jt["name"] = pi.contactName;

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

        mSearchPartners[databaseName] = ja;
        jrep["new"] = !updated;

        // Also refresh struct search cache (search_partner_item), keyed by tenantId.
        QWriteLocker wl(&mPartnersLock);
        for (auto it = mPartners.begin(); it != mPartners.end(); ++it) {
            if (!it.key().endsWith(databaseName) && it.key() != databaseName) {
                continue;
            }
            QVector<PartnerItem> &siv = it.value();
            bool found = false;
            for (int i = 0; i < siv.size(); ++i) {
                if (siv[i].id == pi.id) {
                    siv[i] = pi;
                    found = true;
                    break;
                }
            }
            if (!found) {
                siv.append(pi);
            }
        }
    }

    seLogVerbose(QStringLiteral("searchUpdatePartnerCache db=%1 id=%2 updated=%3 cache=%4 ms=%5")
                     .arg(databaseName)
                     .arg(jo["id"].toInt())
                     .arg(updated ? QStringLiteral("yes") : QStringLiteral("no"))
                     .arg(ja.size())
                     .arg(timer.elapsed()));

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStorage(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StorageItem> &siv = mStorages.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchStorage", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchStorage", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchGoodsItem(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

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
    const int requestedLimit = jo.value("limit").toInt();
    const int limit = requestedLimit > 0 ? qMin(requestedLimit, 500) : 50000;
    const int page = qMax(jo.value("page").toInt(), 0);
    qint64 remainingSkip = barcode.isEmpty() ? static_cast<qint64>(page) * limit : 0;

    QJsonArray jgoods;
    int cacheSize = 0;
    bool hasMore = false;

    {
        QReadLocker rl(&mGoodsLock);
        if (!mGoods.contains(ss.tenantId)) {
            seLogEmptyCache("searchGoodsItem", ss.tenantId);
            seLogSearchResult("searchGoodsItem",
                              ss.tenantId,
                              barcode.isEmpty() ? needle : barcode,
                              0,
                              0,
                              timer.elapsed(),
                              groupId > 0 ? QStringLiteral("group_id=%1").arg(groupId) : QString());
            jrep["result"] = jgoods;
            jrep["page"] = page;
            jrep["limit"] = limit;
            jrep["has_more"] = false;
            jrep["noresult"] = true;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        const QVector<GoodsItem> &siv = mGoods.value(ss.tenantId);
        cacheSize = siv.size();

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

            if (remainingSkip > 0) {
                --remainingSkip;
                continue;
            }

            // Read one matching item beyond the page to report has_more.
            if (jgoods.size() >= limit) {
                hasMore = true;
                break;
            }

            jgoods.append(si.toJson());

            if (!barcode.isEmpty()) {
                break;
            }
        }
    }

    jrep["result"] = jgoods;
    jrep["page"] = page;
    jrep["limit"] = limit;
    jrep["has_more"] = hasMore;
    jrep["noresult"] = jgoods.isEmpty();

    seLogSearchResult("searchGoodsItem",
                      ss.tenantId,
                      barcode.isEmpty() ? needle : barcode,
                      cacheSize,
                      jgoods.size(),
                      timer.elapsed(),
                      QStringLiteral("group_id=%1 page=%2 limit=%3 has_more=%4")
                          .arg(groupId)
                          .arg(page)
                          .arg(limit)
                          .arg(hasMore ? QStringLiteral("yes") : QStringLiteral("no")));

    if(cacheSize == 0) {
        seLogEmptyCache("searchGoodsItem", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchPartnerItem(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jpartners;
    int cacheSize = 0;
    {
        QReadLocker rl(&mPartnersLock);
        const QVector<PartnerItem> &siv = mPartners.value(ss.tenantId);
        cacheSize = siv.size();
        const int limit = 50000;

        for (const PartnerItem &si : siv) {
            if (!partnerMatchesQuery(si, qwords)) {
                continue;
            }

            jpartners.append(si.toJson());

            if (jpartners.size() >= limit)
                break;
        }
    }
    jrep["result"] = jpartners;

    seLogSearchResult("searchPartnerItem", ss.tenantId, needle, cacheSize, jpartners.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchPartnerItem", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::updateDictionary(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString entity = jo.value("entity").toString();

    if (!jo.contains("op")) {
        seLogWarning(QStringLiteral("updateDictionary missing op entity=%1 key=%2").arg(entity, ss.tenantId));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "What do you want to do with dictionary?";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    QString op = jo.value("op").toString();

    if (op != "i" && op != "u") {
        seLogWarning(QStringLiteral("updateDictionary invalid op=%1 entity=%2 key=%3").arg(op, entity, ss.tenantId));
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Invalid operation";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    seLogVerbose(QStringLiteral("updateDictionary start entity=%1 op=%2 id=%3 key=%4 db=%5")
                     .arg(entity, op)
                     .arg(jo.value("id").toInt())
                     .arg(ss.tenantId, ss.databaseName));

    Database db;

    if (!db.open("127.0.0.1", ss.databaseName, "root", "root5", 3306)) {
        LogWriterError(QStringLiteral("C5SearchEngine::updateDictionary open failed key=%1 db=%2 err=%3")
                           .arg(ss.tenantId, ss.databaseName, db.lastDbError()));
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
            gi.groupId = db.integer("f_group_id");
            gi.groupName = db.string("f_group_name");
            gi.name = db.string("f_name");
            gi.barcode = db.string("f_scancode");
            gi.unitName = db.string("f_unit_name");
            gi.lastInputPrice = db.doubleValue("f_lastinput");
            gi.price1 = db.doubleValue("f_price1");
            gi.price1disc = db.doubleValue("f_price1disc");
            gi.price2 = db.doubleValue("f_price2");
            gi.price2disc = db.doubleValue("f_price2disc");
            gi.adgt = db.string("f_adgt");
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
        SELECT gr.f_name AS f_group_name, u.f_group, u.f_first, u.f_last, u.f_login, u.f_phone, u.f_id
        FROM s_user u
        LEFT JOIN s_user_group gr ON gr.f_id=u.f_group
        WHERE u.f_state=1 and u.f_id=:f_id
        )");
        StructEmployee se;

        if (db.next()) {
            se.id = db.integer("f_id");
            se.groupId = db.integer("f_group");
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
    } else if (entity == "partners") {
        QWriteLocker wl(&mPartnersLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(R"(
        select p.f_id, p.f_taxcode, p.f_taxname, p.f_name, p.f_phone
        from c_partners p
        where p.f_id=:f_id
        )");
        PartnerItem pi;

        if (db.next()) {
            pi = makePartnerItem(db.integer("f_id"),
                                 db.string("f_taxcode"),
                                 db.string("f_taxname"),
                                 db.string("f_name"),
                                 db.string("f_phone"));
        } else {
            QString err = QString("Invalid partner with id=%1").arg(jo.value("id").toInt());
            LogWriterError(err);
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = err;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<PartnerItem> &siv = mPartners[ss.tenantId];
        bool updated = false;

        for (int i = 0; i < siv.size(); i++) {
            if (siv[i].id == pi.id) {
                siv[i] = pi;
                updated = true;
                break;
            }
        }

        if (!updated) {
            siv.append(pi);
        }

        // Keep legacy JSON partner cache in sync (search_partner / search_update_partner_cache).
        QJsonArray &ja = mSearchPartners[ss.databaseName];
        QJsonObject jt;
        jt["id"] = pi.id;
        jt["taxname"] = pi.taxName;
        jt["tin"] = pi.tin;
        jt["phone"] = pi.phone;
        jt["contact"] = pi.contactName;
        jt["name"] = pi.contactName;
        jt["address"] = QString();
        jt["discount"] = 0;
        jt["mode"] = 0;
        bool legacyUpdated = false;

        for (int i = 0; i < ja.size(); i++) {
            if (ja.at(i).toObject().value("id").toInt() == pi.id) {
                ja[i] = jt;
                legacyUpdated = true;
                break;
            }
        }

        if (!legacyUpdated) {
            ja.append(jt);
        }
    } else if (entity == "goods_type") {
        QWriteLocker wl(&mGoodsTypeLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(R"(
        SELECT gt.f_id,
               COALESCE(NULLIF(TRIM(l.f_value), ''), CAST(gt.f_id AS CHAR)) AS f_type_name
        FROM c_goods_type gt
        LEFT JOIN l_dictionary l ON l.f_dict='c_goods_type' AND l.f_lang='hy' AND l.f_dict_id=gt.f_id
        WHERE gt.f_id=:f_id
        )");
        StructGoodsType gt;

        if (db.next()) {
            gt.id = db.integer("f_id");
            gt.name = db.string("f_type_name");
            gt.nameLower = gt.name.toLower();
            gt.words = gt.nameLower.split(" ", Qt::SkipEmptyParts);
        } else {
            QString err = QString("Invalid goods type id=%1").arg(jo.value("id").toInt());
            LogWriterError(err);
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = err;
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<StructGoodsType> &siv = mGoodsType[ss.tenantId];
        bool updated = false;

        for (int i = 0; i < siv.size(); i++) {
            if (siv[i].id == gt.id) {
                siv[i] = gt;
                updated = true;
                break;
            }
        }

        if (!updated) {
            siv.append(gt);
        }
    }

    jrep["status"] = 1;

    seLogVerbose(QStringLiteral("updateDictionary done entity=%1 op=%2 id=%3 key=%4 ms=%5")
                     .arg(entity, op)
                     .arg(jo.value("id").toInt())
                     .arg(ss.tenantId)
                     .arg(timer.elapsed()));

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStoreDocStatus(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StoreDocStatusItem> &siv = mStoreDocStatus.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchStoreDocStatus", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchStoreDocStatus", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchStoreDocType(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mStoragesLock);
        const QVector<StoreDocTypeItem> &siv = mStoreDocType.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchStoreDocType", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchStoreDocType", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchCurrency(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mCurrencyLock);
        const QVector<StructCurrency> &siv = mCurrency.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchCurrency", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchCurrency", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchCashbox(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mCashboxLock);
        const QVector<StructCashbox> &siv = mCashbox.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchCashbox", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchCashbox", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchPaymentType(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    int cacheSize = 0;
    {
        QReadLocker rl(&mPaymentTypeLock);
        const QVector<StructPaymentType> &siv = mPaymentType.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchPaymentType", ss.tenantId, needle, cacheSize, jstorages.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchPaymentType", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchGoodsType(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jtypes;
    int cacheSize = 0;
    {
        QReadLocker rl(&mGoodsTypeLock);
        const QVector<StructGoodsType> &siv = mGoodsType.value(ss.tenantId);
        cacheSize = siv.size();
        const int limit = 50000;

        for (auto const &si : siv) {
            bool match = true;

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

            if (!match)
                continue;

            jtypes.append(QJsonObject{{"f_id", si.id}, {"f_name", si.name}});

            if (jtypes.size() >= limit)
                break;
        }
    }
    jrep["result"] = jtypes;

    seLogSearchResult("searchGoodsType", ss.tenantId, needle, cacheSize, jtypes.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchGoodsType", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchEmployee(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jemployees;
    int cacheSize = 0;
    {
        QReadLocker rl(&mEmployeeLock);
        const QVector<StructEmployee> &siv = mEmployees.value(ss.tenantId);
        cacheSize = siv.size();
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
                                          {"f_group", si.groupId},
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

    seLogSearchResult("searchEmployee", ss.tenantId, needle, cacheSize, jemployees.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchEmployee", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchEmployeeGroup(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jgroups;
    int cacheSize = 0;
    {
        QReadLocker rl(&mEmployeeGroupLock);
        const QVector<StructEmployeeGroup> &siv = mEmployeeGroups.value(ss.tenantId);
        cacheSize = siv.size();
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

    seLogSearchResult("searchEmployeeGroup", ss.tenantId, needle, cacheSize, jgroups.size(), timer.elapsed());

    if(cacheSize == 0) {
        seLogEmptyCache("searchEmployeeGroup", ss.tenantId);
    }

    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}

QString C5SearchEngine::searchUpdateGoodsLastInputPrices(const QJsonObject &jo, const SocketStruct &ss)
{
    QElapsedTimer timer;
    timer.start();

    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    QJsonArray items;
    const QJsonValue dataVal = jo.value("data");
    if (dataVal.isArray()) {
        items = dataVal.toArray();
    } else if (dataVal.isString()) {
        const QJsonDocument doc = QJsonDocument::fromJson(dataVal.toString().toUtf8());
        if (!doc.isArray()) {
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = "Invalid data format: expected array of items";
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }
        items = doc.array();
    } else {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Invalid data format: expected array of items";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }
    int updatedCount = 0;
    int cacheSize = 0;

    {
        // Блокируем кэш товаров на запись
        QWriteLocker locker(&mGoodsLock);

        if (!mGoods.contains(ss.tenantId)) {
            jrep["errorCode"] = 1;
            jrep["errorMessage"] = "Tenant cache not found";
            seLogWarning(QStringLiteral("searchUpdateGoodsLastInputPrices tenant cache not found key=%1 db=%2 ms=%3")
                             .arg(ss.tenantId, ss.databaseName)
                             .arg(timer.elapsed()));
            return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        }

        QVector<GoodsItem> &goodsVector = mGoods[ss.tenantId];
        QHash<int, int> &indexMap = mGoodsIndex[ss.tenantId];
        cacheSize = goodsVector.size();

        for (int i = 0; i < items.size(); ++i) {
            QJsonObject itemObj = items.at(i).toObject();
            int itemId = itemObj.value("item_id").toInt();
            double lastPrice = itemObj.value("price").toVariant().toDouble();
            if (lastPrice <= 0.0 && itemObj.contains("f_price")) {
                lastPrice = itemObj.value("f_price").toVariant().toDouble();
            }

            if (itemId <= 0 || lastPrice <= 0.0) {
                continue;
            }
            if (indexMap.contains(itemId)) {
                int vectorIndex = indexMap.value(itemId);
                if (vectorIndex >= 0 && vectorIndex < goodsVector.size()) {
                    goodsVector[vectorIndex].lastInputPrice = lastPrice;
                    updatedCount++;
                }
            }
        }
    }

    seLogVerbose(QStringLiteral("searchUpdateGoodsLastInputPrices key=%1 db=%2 items=%3 updated=%4 cache=%5 ms=%6")
                     .arg(ss.tenantId, ss.databaseName)
                     .arg(items.size())
                     .arg(updatedCount)
                     .arg(cacheSize)
                     .arg(timer.elapsed()));

    if(cacheSize == 0) {
        seLogEmptyCache("searchUpdateGoodsLastInputPrices", ss.tenantId);
    }

    jrep["updated_count"] = updatedCount;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}
