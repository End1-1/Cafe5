#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QHash>

class RequestHandler;

class RequestManager
{
public:
    RequestManager();
    static void init();
    static RequestHandler *getHandler(const QString &route);
    static void releaseHandler(RequestHandler *rh);

private:
    QHash<QString, QList<RequestHandler*> > fHandlers;
    static RequestManager *fInstance;
    RequestHandler *findHandler(const QString &route);
    RequestHandler *createHandler(const QString &route);
};

#endif // REQUESTMANAGER_H
