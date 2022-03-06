#include "requesthandler.h"
#include "database.h"
#include "requestmanager.h"
#include "logwriter.h"
#include "databaseconnectionmanager.h"
#include <QElapsedTimer>

RequestHandler::RequestHandler()
{
    fIdle = false;
    fContentType = ContentType::MultipartFormData;
    fCreated = QDateTime::currentDateTime();
}

RequestHandler::~RequestHandler()
{
}

RequestHandler *RequestHandler::route(const QString &session, const QString &remoteHost, const QString &r, const QByteArray &data, const QHash<QString, DataAddress> &dataMap, ContentType contentType)
{
    QElapsedTimer et;
    et.start();
    LogWriter::write(LogWriterLevel::verbose, session, "New request from " + remoteHost);
    RequestHandler *rh = RequestManager::getHandler(r);
    rh->fSession = session;
    rh->fContentType = contentType;
    emit rh->sendData(0, rh->fSession, QString("Request data: %1").arg(QString(data)), QVariant());
    if (rh->validateData(data, dataMap)) {
        rh->handle(data, dataMap);
    }
    int ms = et.elapsed();
    emit rh->sendData(0, rh->fSession, QString("Request handler(%1ms). %2").arg(ms).arg(r), QVariant());
    Database db;
    JsonHandler jh;
    if (DatabaseConnectionManager::openDatabase(db, jh)) {
        db[":fdate"] = QDate::currentDate();
        db[":ftime"] = QTime::currentTime();
        db[":fhost"] = remoteHost;
        db[":felapsed"] = ms;
        db[":froute"] = r;
        db.insert("system_requests");
    } else {
        LogWriter::write(LogWriterLevel::errors, session, db.lastDbError());
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

long RequestHandler::liveDuration()
{
    return fCreated.secsTo(QDateTime::currentDateTime());
}

bool RequestHandler::setResponse(int responseCode, const QString &data)
{
    fHttpHeader.setResponseCode(responseCode);
    fHttpHeader.setContentLength(data.toUtf8().length());
    fResponse.append(fHttpHeader.toString());
    fResponse.append(data.toUtf8());
    emit sendData(0, fSession, "http response: " + data, QVariant());
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
