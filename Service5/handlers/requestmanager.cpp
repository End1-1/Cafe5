#include "requestmanager.h"
#include "authentication.h"
#include "registration.h"
#include "notfound.h"
#include "forbidden.h"
#include "confirmregistration.h"
#include "shoprequest.h"
#include "storerequest.h"
#include "chat.h"
#include "jzstore.h"
#include "tax.h"
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
    fInstance->fHandlers.insert("/storerequest", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/shoprequest", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/jzstore", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/chat", QList<RequestHandler*>());
    fInstance->fHandlers.insert("/printtax", QList<RequestHandler*>());
}

RequestHandler *RequestManager::getHandler(const QString &route)
{
    if (!fInstance->fHandlers.contains(route)) {
        NotFound *rh = new NotFound(route);
        return rh;
    }
    return fInstance->findHandler(route);
}

void RequestManager::releaseHandler(RequestHandler *rh)
{
//    QMutexLocker ml(&fMutex);
//    rh->setIdle(true);
    delete rh;
}

RequestHandler *RequestManager::findHandler(const QString &route)
{
//    QElapsedTimer t;
//    t.start();

//    QMutexLocker ml(&fMutex);
//    QList<RequestHandler*> &l = fHandlers[route];
//    RequestHandler *result = nullptr;
//    for (RequestHandler *r: l) {
//        if (!result) {
//            if (r->idle()) {
//                r->setIdle(false);
//                result = r;
//            }
//        } else {

//        }
//    }


    RequestHandler *rh = createHandler(route);
    //l.append(rh);
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
    } else if (route == "/storerequest") {
        rh = new StoreRequest();
    } else if (route == "/shoprequest") {
        rh = new ShopRequest();
    } else if (route == "/notfound") {
        rh = new NotFound(route);
    } else if (route == "/jzstore") {
        rh = new JZStore();
    } else if (route == "/chat") {
        rh = new Chat();
    } else if (route == "/printtax") {
        rh = new Tax();
    } else {
        rh = new NotFound(route);
    }
    return rh;
}
