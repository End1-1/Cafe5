#include "requestmanager.h"
#include "authentication.h"
#include "registration.h"
#include "notfound.h"
#include "forbidden.h"
#include "confirmregistration.h"
#include <QMutex>
#include <QElapsedTimer>

static QMutex fMutex;

RequestManager *RequestManager::fInstance = 0;

RequestManager::RequestManager()
{

}

void RequestManager::init()
{
    fInstance = new RequestManager();
    fInstance->fHandlers.insert("/register", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/confirmregistration", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/authentication", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/notfound", QList<RequestHandler*>());
}

RequestHandler *RequestManager::getHandler(const QString &route)
{
    if (!fInstance->fHandlers.contains(route)) {
        RequestHandler *rh = fInstance->findHandler("/notfound");
        return rh;
    }
    return fInstance->findHandler(route);
}

void RequestManager::releaseHandler(RequestHandler *rh)
{
    //QMutexLocker ml(&fMutex);
    rh->setIdle(true);
}

RequestHandler *RequestManager::findHandler(const QString &route)
{
    QElapsedTimer t;
    t.start();

    QMutexLocker ml(&fMutex);
    QList<RequestHandler*> &l = fHandlers[route];
    for (RequestHandler *r: l) {
        if (r->idle()) {
            r->setIdle(false);
            __debug_log("Find handler exists");
            __debug_log(t.elapsed());
            return r;
        }
    }


    RequestHandler *rh = createHandler(route);
    l.append(rh);
    __debug_log("Find handler new");
    __debug_log(t.elapsed());
    return rh;
}

RequestHandler *RequestManager::createHandler(const QString &route)
{
    RequestHandler *rh = nullptr;
    if (route == "/authentication") {
        rh = new Authentication();
    } else if (route == "/register") {
        rh = new Registration();
    } else if (route == "/confirmregistration") {
        rh = new ConfirmRegistration();
    } else if (route == "/notfound") {
        rh = new NotFound(route);
    } else {
        rh = new NotFound(route);
    }
    return rh;
}
