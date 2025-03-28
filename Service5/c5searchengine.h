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
    void search(const QJsonObject &jo, QWebSocket *socket);
    void searchPartner(const QJsonObject &jo, QWebSocket *socket);
    void searchGoodsGroups(const QJsonObject &jo, QWebSocket *socket);
    void searchGoods(const QJsonObject &jo, QWebSocket *socket);
    void searchStore(const QJsonObject &jo, QWebSocket *socket);
    void searchUpdatePartnerCache(const QJsonObject &jo, QWebSocket *socket);

};

#endif // C5SEARCHENGINE_H
