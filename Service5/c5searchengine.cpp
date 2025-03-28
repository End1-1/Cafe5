#include "c5searchengine.h"
#include "logwriter.h"
#include "database.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

struct SearchObject {
    int mode;
    int objectId;
    QString name;
};
// Database - > List of words
QMap<QString, QStringList> mSearchStrings;

// Database -> Index of word -> SearchObject
QMap<QString, QMap<int, SearchObject> > mSearchObjects;

C5SearchEngine *C5SearchEngine::mInstance = nullptr;

QMap<QString, QJsonArray> mSearchPartners;
QMap<QString, QJsonArray> mSearchGoodsGroups;
QMap<QString, QJsonArray> mSearchGoods;

C5SearchEngine::C5SearchEngine() :
    QObject()
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
    for (const QString &dbname : databases) {
        int dbitemscount = 0;
        if (!db.open("127.0.0.1", dbname, "root", "root5")) {
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
            //LogWriter::write(LogWriterLevel::special, "", db.string("f_word"));
            words.append(db.string("f_word"));
            objects[dbitemscount++] = {db.integer("f_mode"), db.integer("f_id"),  db.string("f_orig")};
        }
        totalitems += dbitemscount;
        //FILL PARTNERS MAP
        sql = QString::fromStdString(R"sql(
        select f_id, f_taxname, coalesce(f_taxcode, '') as f_taxcode, f_price_politic,
        f_address, f_permanent_discount,
        f_phone, f_contact
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
            ja.append(jt);
        }
        mSearchPartners[dbname] = ja;
        //FILL GOODS GROUPS
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
        //FILL GOODS MAP
        sql = QString::fromStdString(R"sql(
        select g.f_id, gr.f_id as f_groupid, g.f_name, gr.f_name as f_groupname, u.f_name as f_unitname,
        gpr.f_price1, gpr.f_price1disc, gpr.f_price2, gpr.f_price2disc, 0 as f_qty
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
            jt["qty"] = 0;
            jgoods.append(jt);
        }
        mSearchGoods[dbname] = jgoods;
    }
    qDebug() << "serch engine of dishes initialized ";
    qDebug() << "total items" << totalitems;
}

void C5SearchEngine::search(const QJsonObject &jo, QWebSocket *socket)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    QString repMsg;
    if (jo["template"].toString().isEmpty()) {
        jrep["result_count"] = 0;
        repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        socket->sendTextMessage(repMsg);
        return;
    }
    int maxCount = jo["max_count"].toInt() == 0 ? 10 :  jo["max_count"].toInt();
    QString databaseName = jo["database"].toString();
    const QStringList &words = mSearchStrings[databaseName];
    QStringList templateWords = jo["template"].toString().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    templateWords.append(jo["template"].toString());
    QList<int> foundedIndexes;
    bool stopflag = false;
    for (const QString &searchString : std::as_const(templateWords)) {
        for (int i = 0; i < words.count(); i++) {
            if (words.at(i).contains(QRegularExpression("\\b" + QRegularExpression::escape(searchString),
                                     QRegularExpression::UseUnicodePropertiesOption))) {
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
        ja.append(QJsonObject{{"mode", objects[i].mode},
            {"id", objects[i].objectId},
            {"name", objects[i].name}});
    }
    jrep["result_count"] = ja.count();
    jrep["result"] = ja;
    repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    socket->sendTextMessage(repMsg);
}

void C5SearchEngine::searchPartner(const QJsonObject &jo, QWebSocket *socket)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    const QJsonArray &src = mSearchPartners[jo["database"].toString()];
    int page = jo["page"].toInt();
    int limit = jo["limit"].toInt();
    int skip = page *limit;
    QJsonArray result;
    QString searchString = jo["template"].toString();
    bool noResult = true;
    for (int i = 0; i < src.size(); i++) {
        const QJsonObject &jt = src.at(i).toObject();
        if (jt["taxname"].toString().contains(searchString, Qt::CaseInsensitive)
                || jt["tin"].toString().contains(searchString, Qt::CaseInsensitive)
                || jt["phone"].toString().contains(searchString, Qt::CaseInsensitive)
                || jt["contact"].toString().contains(searchString, Qt::CaseInsensitive)) {
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
    QString repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    socket->sendTextMessage(repMsg);
}

void C5SearchEngine::searchGoodsGroups(const QJsonObject &jo, QWebSocket *socket)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
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
    QString repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    socket->sendTextMessage(repMsg);
}

void C5SearchEngine::searchGoods(const QJsonObject &jo, QWebSocket *socket)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    const QJsonArray &src = mSearchGoods[jo["database"].toString()];
    QJsonArray result;
    QString searchString = jo["template"].toString();
    QJsonArray jsrc;
    if (jo["groupid"].toInt() > 0) {
        for (int i = 0; i < src.size(); i++) {
            const QJsonObject &jt = src.at(i).toObject();
            if (jt["groupid"].toInt() == jo["groupid"].toInt()) {
                jsrc.append(jt);
            }
        }
    } else {
        jsrc = src;
    }
    int page = jo["page"].toInt();
    int limit = jo["limit"].toInt();
    int skip = page *limit;
    bool noResult = true;
    for (int i = 0; i < jsrc.size(); i++) {
        const QJsonObject &jt = jsrc.at(i).toObject();
        if (jt["name"].toString().contains(searchString, Qt::CaseInsensitive)) {
            noResult = false;
            if (skip > 0) {
                skip--;
                continue;
            }
            result.append(jt);
        }
        if (result.size() == limit) {
            break;
        }
    }
    jrep["result_count"] = result.size();
    jrep["result"] = result;
    jrep["page"] = jo["page"];
    jrep["limit"] = jo["limit"];
    jrep["noresult"] = noResult;
    QString repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    socket->sendTextMessage(repMsg);
}

void C5SearchEngine::searchStore(const QJsonObject &jo, QWebSocket *socket)
{
    QString repMsg;
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    QJsonArray result;
    Database db;
    if (!db.open("127.0.0.1", jo["database"].toString(), "root", "root5")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        socket->sendTextMessage(repMsg);
        return;
    }
    repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
    LogWriter::write(LogWriterLevel::verbose, "", repMsg);
    socket->sendTextMessage(repMsg);
}

void C5SearchEngine::searchUpdatePartnerCache(const QJsonObject &jo, QWebSocket *socket)
{
    QJsonObject jrep;
    jrep["errorCode"] = 0;
    jrep["requestId"] = jo["requestId"];
    QString repMsg;
    QString sql = QString::fromStdString(R"sql(
        select f_id, f_taxname, coalesce(f_taxcode, '') as f_taxcode, f_price_politic,
        f_address, f_permanent_discount,
        f_phone, f_contact
        from c_partners where f_state>0 and f_id=:f_id
        order by 2
        )sql");
    Database db;
    if (!db.open("127.0.0.1", jo["database"].toString(), "root", "root5")) {
        jrep["errorCode"] = 1;
        jrep["errorMessage"] = db.lastDbError();
        LogWriter::write(LogWriterLevel::errors, "", repMsg);
        socket->sendTextMessage(repMsg);
        return;
    }
    db[":f_id"] = jo["id"].toInt();
    db.exec(sql);
    QJsonArray ja = mSearchPartners[jo["database"].toString()];;
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
        QString repMsg = QJsonDocument(jrep).toJson(QJsonDocument::Compact);
        LogWriter::write(LogWriterLevel::verbose, "", repMsg);
        socket->sendTextMessage(repMsg);
    }
}
