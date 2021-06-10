#include "requesthandler.h"
#include "database.h"
#include "requestmanager.h"
#include <QDateTime>
#include <QElapsedTimer>

RequestHandler::RequestHandler()
{
    fIdle = false;
    fContentType = ContentType::MultipartFormData;
}

RequestHandler::~RequestHandler()
{
    __debug_log("~RequestHandler");
}

RequestHandler *RequestHandler::route(const QString &remoteHost, const QString &r, const QByteArray &data, const QHash<QString, DataAddress> &dataMap, ContentType contentType)
{
    QElapsedTimer et;
    et.start();
    __debug_log("New request from " + remoteHost);
    RequestHandler *rh = RequestManager::getHandler(r);
    rh->fContentType = contentType;
    if (rh->validateData(data, dataMap)) {
        rh->handle(data, dataMap);
    }
    Database db;
    if (db.open("127.0.0.1", "server5", "root", "osyolia")) {
        db[":fdate"] = QDate::currentDate();
        db[":ftime"] = QTime::currentTime();
        db[":fhost"] = remoteHost;
        db[":felapsed"] = et.elapsed();
        db[":froute"] = r;
        db.insert("system_requests");
    } else {
        __debug_log(db.lastDbError());
    }
    return rh;
}

bool RequestHandler::idle()
{
    return fIdle;
}

void RequestHandler::setIdle(bool v)
{
    fIdle = v;
    if (fIdle) {
        fHttpHeader.reset();
        fResponse.clear();
    }
}

bool RequestHandler::setResponse(int responseCode, const QString &data)
{
    fHttpHeader.setResponseCode(responseCode);
    fHttpHeader.setContentLength(data.toUtf8().length());
    fResponse.append(fHttpHeader.toString());
    fResponse.append(data.toUtf8());
    return true;
}

bool RequestHandler::setInternalServerError(const QString &msg)
{
    setResponse(HTTP_INTERNAL_SERVER_ERROR, msg);
    return false;
}

bool RequestHandler::setForbiddenError(const QString &msg)
{
    setResponse(HTTP_FORBIDDEN, msg);
    return false;
}

bool RequestHandler::setDataValidationError(const QString &msg)
{
    setResponse(HTTP_DATA_VALIDATION_ERROR, msg);
    return false;
}
