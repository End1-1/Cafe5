#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "socketdata.h"
#include <QHash>

typedef void (*dllroutes)(QStringList &r);
typedef bool (*handle_route) (const QByteArray &indata, QByteArray&outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

class RequestHandler;

class RequestManager
{
public:
    RequestManager();
    static void init();
    static handle_route getRouteHandler(const QString &route);
    static void handle(const QString &session, const QString &remoteHost, const QString &r, const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, ContentType contentType);

private:
    QHash<QString, QList<RequestHandler*> > fHandlers;
    static RequestManager *fInstance;
    QHash<QString, QString> fRouteDll;
    QHash<QString, handle_route> fRouteFunction;
};

#endif // REQUESTMANAGER_H
