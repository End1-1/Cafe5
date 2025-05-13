#ifndef C5SEARCHENGINE_H
#define C5SEARCHENGINE_H

#include <QObject>
#include <QWebSocket>

class C5SearchEngine : public QObject
{
public:
    C5SearchEngine();
    static C5SearchEngine *mInstance;
    static void init(QStringList databases);
    QString search(const QJsonObject &jo);
    QString searchPartner(const QJsonObject &jo);
    QString searchGoodsGroups(const QJsonObject &jo);
    QString searchGoods(const QJsonObject &jo);
    QString searchStore(const QJsonObject &jo);
    QString searchUpdatePartnerCache(const QJsonObject &jo);

};

#endif // C5SEARCHENGINE_H
