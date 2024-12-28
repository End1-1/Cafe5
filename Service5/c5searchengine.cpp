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
    for (const QString &dbname : databases) {
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
        WHERE LENGTH(f_word)>0
        ORDER BY f_mode desc, f_word
        )sql");
        db.exec(sql);
        while (db.next()) {
            //LogWriter::write(LogWriterLevel::special, "", db.string("f_word"));
            words.append(db.string("f_word"));
            objects[totalitems++] = {db.integer("f_mode"), db.integer("f_id"),  db.string("f_orig")};
        }
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
    LogWriter::write(LogWriterLevel::errors, "", repMsg);
    socket->sendTextMessage(repMsg);
}
