#include "c5searchengine.h"
#include "logwriter.h"
#include "database.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
C5SearchEngine::C5SearchEngine()
{
}

void C5SearchEngine::init(QStringList databases)
{
    Database db;
    int totalitems = 0;
    for (const QString &dbname : databases) {
        if (!db.open("127.0.0.1", dbname, "root", "root5")) {
            continue;
        }
        mSearchStrings[dbname] = QStringList();
        mSearchObjects[dbname] = QMap<int, SearchObject>();
        QStringList &words = mSearchStrings[dbname];
        QMap<int, SearchObject> &objects = mSearchObjects[dbname];
        db.exec("SELECT f_id, f_mode, f_word as f_orig, lower(f_word) as f_word FROM ( "
                "SELECT f_id, f_mode, f_en AS f_word FROM d_translator "
                "UNION "
                "SELECT f_id, f_mode, f_ru AS f_word FROM d_translator "
                "union "
                "SELECT f_id, f_mode, f_am AS f_word FROM d_translator "
                ") AS tr "
                "WHERE LENGTH(f_word)>0 "
                "ORDER BY f_mode desc, f_word");
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
    int maxCount = 10;
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
