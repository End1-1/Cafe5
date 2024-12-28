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
};

#endif // C5SEARCHENGINE_H
