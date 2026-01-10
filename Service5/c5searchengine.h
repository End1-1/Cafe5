#ifndef C5SEARCHENGINE_H
#define C5SEARCHENGINE_H

#include "socketstruct.h"

#include <QObject>
#include <QWebSocket>

class C5SearchEngine : public QObject
{
public:
    C5SearchEngine();
    static C5SearchEngine* mInstance;
    static void init(QStringList databases);
    static void init(const QString &databaseName, const QString &serverKey);
    QString search(const QJsonObject &jo);
    QString searchPartner(const QJsonObject &jo);
    QString searchGoodsGroups(const QJsonObject &jo);
    QString searchGoods(const QJsonObject &jo);
    QString searchStore(const QJsonObject &jo);
    QString searchUpdatePartnerCache(const QJsonObject &jo);
    //New version of search
    QString searchStorage(const QJsonObject &jo, const SocketStruct &ss);
    QString searchGoodsItem(const QJsonObject &jo, const SocketStruct &ss);
    QString searchPartnerItem(const QJsonObject &jo, const SocketStruct &ss);

private:

};

#endif // C5SEARCHENGINE_H
