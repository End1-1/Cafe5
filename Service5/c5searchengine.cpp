#include "c5searchengine.h"
#include "database.h"
#include "socketstruct.h"
#include "struct_storage_item.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "logwriter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWriteLocker>
#include <QTimer>

static const QString mSqlGoods = R"(
    select g.f_id,  gr.f_name as f_group_name, g.f_name, g.f_scancode,
    u.f_name as f_unit_name
    from c_goods g
    left join c_groups gr on gr.f_id=g.f_group
    left join c_units u on u.f_id=g.f_unit
    %where%
    order by g.f_name
    )";

QReadWriteLock mStoragesLock;
QReadWriteLock mGoodsLock;
QReadWriteLock mPartnersLock;
QHash<QString, QVector<StorageItem>> mStorages;
QHash<QString, QVector<GoodsItem>> mGoods;
QHash<QString, QHash<int, int>> mGoodsIndex;
QHash<QString, QVector<PartnerItem>> mPartners;

struct SearchObject {
    int mode;
    int objectId;
    QString name;
};
// Database - > List of words
QMap < QString, QStringList > mSearchStrings;
// Database -> Index of word -> SearchObject
QMap < QString, QMap < int, SearchObject> > mSearchObjects;
C5SearchEngine * C5SearchEngine::mInstance = nullptr;
QMap < QString, QJsonArray > mSearchPartners;
QMap < QString, QJsonArray > mSearchGoodsGroups;
QMap < QString, QJsonArray > mSearchGoods;

C5SearchEngine::C5SearchEngine() : QObject()
{
}

void C5SearchEngine::init(QStringList databases)
{
    Database db;
    int totalitems = 0;
    mSearchObjects.clear();
    mSearchStrings.clear();
    mSearchPartners.clear();
    mSearchGoodsGroups.clear();
    mSearchGoods.clear();

    for(const QString &dbname : databases) {
        int dbitemscount = 0;
        int port = 3306;
#ifdef QT_DEBUG
        port = 3306;
#endif

        if(!db.open("127.0.0.1", dbname, "root", "root5", port)) {
            continue;
        }

        mSearchStrings[dbname] = QStringList();
        mSearchObjects[dbname] = QMap < int, SearchObject > ();
        QStringList &words = mSearchStrings[dbname];
        QMap < int, SearchObject > & objects = mSearchObjects[dbname];
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

        while(db.next()) {
            // LogWriter::write(LogWriterLevel::special, "",
            // db.string("f_word"));
            words.append(db.string("f_word"));
            objects[dbitemscount++] = { db.integer("f_mode"), db.integer("f_id"),
                                        db.string("f_orig")
                                      };
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

        while(db.next()) {
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

        while(db.next()) {
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

        while(db.next()) {
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

    if(!db.open("127.0.0.1", databaseName, "root", "root5", port)) {
        return;
    }

    /* STORAGES */
    QVector < StorageItem > tmp1;
    tmp1.reserve(256);
    db.exec("select f_id, f_name from c_storages order by f_name");

    while(db.next()) {
        QString name = db.string("f_name");
        tmp1.append({
            db.integer("f_id"),
            name,
            name.toLower(),
            name.toLower().split(" ", Qt::SkipEmptyParts)
        });
    }

    /* GOODS */
    QVector < GoodsItem > tmp2;
    QHash < int, int > tmplIndex2;
    tmp2.reserve(4096);
    db.exec(QString(mSqlGoods).replace("%where%", ""));

    while(db.next()) {
        QString name = db.string("f_group_name") + " " + db.string("f_name") + " " + db.string("f_scancode");
        GoodsItem g;
        g.id        = db.integer("f_id");
        g.groupName = db.string("f_group_name");
        g.name      = db.string("f_name");
        g.unitName  = db.string("f_unit_name");
        g.barcode   = db.string("f_barcode");
        g.lastInputPrice = db.doubleValue("f_lastinput");
        g.price1    = db.doubleValue("f_price1");
        g.price1disc = db.doubleValue("f_price1disc");
        g.price2    = db.doubleValue("f_price2");
        g.price2disc = db.doubleValue("f_price2disc");
        g.name = name;
        g.words = name.toLower().split(" ", Qt::SkipEmptyParts);
        tmp2.append(g);
        tmplIndex2[tmp2.last().id] = tmp2.size() - 1;
    }

    /* PARTNERS */
    QVector < PartnerItem > tmp3;
    tmp3.reserve(256);
    db.exec(R"(
    select p.f_id, p.f_taxcode, p.f_taxname, p.f_name, p.f_phone
    from c_partners p
    order by p.f_name
    )");

    while(db.next()) {
        QString name = QString("%1 %2 %3 %4").arg(
                           db.string("f_taxcode"),
                           db.string("f_taxname"),
                           db.string("f_name"),
                           db.string("f_phone")).toLower();
        tmp3.append({
            db.integer("f_id"),
            db.string("f_taxcode"),
            db.string("f_taxname"),
            db.string("f_name"),
            db.string("f_phone"),
            name,
            name.split(" ", Qt::SkipEmptyParts)
        });
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

    qDebug() << "storages of" << databaseName << mStorages[serverKey].count() << "items";
    qDebug() << "goods of" << databaseName << mGoods[serverKey].count() << "items";
    qDebug() << "partners of " << databaseName << mPartners[serverKey].count() << "items";
}

QString C5SearchEngine::search(const QJsonObject &jo)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    jrep["actionId"] = jo["actionId"];

    if(jo["template"].toString().isEmpty()) {
        jrep["result_count"] = 0;
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    int maxCount = jo["max_count"].toInt() == 0 ? 10 : jo["max_count"].toInt();
    QString databaseName = jo["database"].toString();
    const QStringList &words = mSearchStrings[databaseName];
    QStringList templateWords = jo["template"].toString().split(
                                    QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    templateWords.append(jo["template"].toString());
    QList < int > foundedIndexes;
    bool stopflag = false;

    for(const QString &searchString : std::as_const(templateWords)) {
        for(int i = 0; i < words.count(); i++) {
            if(words.at(i).contains(QRegularExpression(
                                        "\\b" + QRegularExpression::escape(searchString),
                                        QRegularExpression::UseUnicodePropertiesOption))) {
                if(!foundedIndexes.contains(i)) {
                    foundedIndexes.append(i);
                }

                if(foundedIndexes.count() >= maxCount) {
                    stopflag = true;
                    break;
                }
            }

            if(stopflag) {
                break;
            }
        }

        if(stopflag) {
            break;
        }
    }

    qDebug() << "search completed with" << foundedIndexes.count() << "result";
    const QMap < int, SearchObject > & objects = mSearchObjects[databaseName];
    QJsonArray ja;

    for(int i : foundedIndexes) {
        ja.append(QJsonObject{ { "mode", objects[i].mode },
            { "id", objects[i].objectId },
            { "name", objects[i].name } });
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

    for(int i = 0; i < src.size(); i++) {
        const QJsonObject &jt = src.at(i).toObject();

        if(jt["taxname"].toString().contains(searchString,
                                             Qt::CaseInsensitive) ||
                jt["tin"].toString().contains(searchString, Qt::CaseInsensitive) ||
                jt["phone"].toString().contains(searchString,
                                                Qt::CaseInsensitive) ||
                jt["contact"].toString().contains(searchString,
                        Qt::CaseInsensitive) ||
                jt["address"].toString().contains(searchString,
                        Qt::CaseInsensitive) ||
                jt["name"].toString().contains(searchString, Qt::CaseInsensitive)) {
            noResult = false;

            if(skip > 0) {
                skip--;
                continue;
            }

            result.append(jt);

            if(result.size() == limit) {
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

    for(int i = 0; i < src.size(); i++) {
        const QJsonObject &jt = src.at(i).toObject();

        if(jt["name"].toString().contains(searchString, Qt::CaseInsensitive)) {
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

    if(!db.open("127.0.0.1", jo["database"].toString(), "root", "root5", port)) {
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

    if(!db.open("127.0.0.1", jo["database"].toString(), "root", "root5", port)) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    db[":f_id"] = jo["id"].toInt();
    db.exec(sql);
    QJsonArray ja = mSearchPartners[jo["database"].toString()];
    ;
    bool updated = false;

    if(db.next()) {
        QJsonObject jt;
        jt["id"] = db.integer("f_id");
        jt["taxname"] = db.string("f_taxname");
        jt["tin"] = db.string("f_taxcode");
        jt["phone"] = db.string("f_phone");
        jt["contact"] = db.string("f_contact");
        jt["address"] = db.string("f_address");
        jt["discount"] = db.doubleValue("f_permanent_discount");
        jt["mode"] = db.integer("f_price_politic");

        for(int i = 0; i < ja.size(); i++) {
            const QJsonObject &jj = ja.at(i).toObject();

            if(jj["id"].toInt() == jt["id"].toInt()) {
                ja[i] = jt;
                updated = true;
                break;
            }
        }

        if(!updated) {
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
        const QVector < StorageItem > & siv = mStorages.value(ss.tenantId);
        const int limit = 50000;

        for(const StorageItem &si : siv) {
            bool match = true;

            for(const QString &qw : qwords) {
                bool found = false;

                for(const QString &w : si.words) {
                    if(w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    match = false;
                    break;
                }
            }

            if(!match)
                continue;

            jstorages.append(QJsonObject{
                {"f_id", si.id},
                {"f_name", si.name}
            });

            if(jstorages.size() >= limit)
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
    QString needle = jo["lower_name"].toString().trimmed();
    QStringList qwords = needle.split(' ', Qt::SkipEmptyParts);
    QJsonArray jstorages;
    {
        QReadLocker rl(&mGoodsLock);
        const QVector < GoodsItem > & siv = mGoods.value(ss.tenantId);
        const int limit = 50000;

        for(const GoodsItem &si : siv) {
            bool match = true;

            if(jo.contains("barcode")) {
                match = si.barcode == jo.value("barcode").toString();

                if(!match) {
                    continue;
                }
            } else {
                for(const QString &qw : qwords) {
                    bool found = false;

                    for(const QString &w : si.words) {
                        if(w.startsWith(qw, Qt::CaseInsensitive)) {
                            found = true;
                            break;
                        }
                    }

                    if(!found) {
                        match = false;
                        break;
                    }
                }

                if(!match)
                    continue;
            }

            jstorages.append(QJsonObject{
                {"f_id", si.id},
                {"f_group_name", si.groupName},
                {"f_name", si.name},
                {"f_barcode", si.barcode},
                {"f_unit_name", si.unitName}
            });

            if(jstorages.size() >= limit) {
                break;
            }

            if(jo.contains("barcode")) {
                break;
            }
        }
    }
    jrep["result"] = jstorages;
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
        const QVector < PartnerItem > & siv = mPartners.value(ss.tenantId);
        const int limit = 50000;

        for(const PartnerItem &si : siv) {
            bool match = true;

            for(const QString &qw : qwords) {
                bool found = false;

                for(const QString &w : si.words) {
                    if(w.startsWith(qw)) {
                        found = true;
                        break;
                    }
                }

                if(!found) {
                    match = false;
                    break;
                }
            }

            if(!match)
                continue;

            jstorages.append(QJsonObject{
                {"f_id", si.id},
                {"f_taxcode", si.tin},
                {"f_taxname", si.taxName},
                {"f_contact", si.contactName},
                {"f_phone", si.phone}
            });

            if(jstorages.size() >= limit)
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

    if(!jo.contains("op")) {
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

    if(!db.open("127.0.0.1", ss.databaseName, "root", "root5", 3306)) {
        LogWriterError(db.lastDbError());
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = "Database error";
        return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    }

    if(entity == "goods") {
        QWriteLocker wl(&mGoodsLock);
        db[":f_id"] = jo.value("id").toInt();
        db.exec(QString(mSqlGoods).replace("%where%", "where g.f_id=:f_id"));
        GoodsItem gi;

        if(db.next()) {
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

        QVector < GoodsItem > & siv = mGoods[ss.tenantId];
        QHash < int, int > &sin = mGoodsIndex[ss.tenantId];

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
    }

    jrep["status"] = 1;
    return QJsonDocument(jrep).toJson(QJsonDocument::Compact);
}
