#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "socketdata.h"
#include "debug.h"
#include "socketdata.h"
#include "httpheader.h"
#include <QByteArray>
#include <QDateTime>
#include <QHash>

class RequestHandler : public QObject
{
    Q_OBJECT

public:
    explicit RequestHandler();
    virtual ~RequestHandler();
    static RequestHandler *route(const QString &session, const QString &remoteHost, const QString &r, const QByteArray &data, const QHash<QString, DataAddress> &dataMap, ContentType contentType);
    QByteArray fResponse;
    ContentType fContentType;
    bool idle();
    void setIdle(bool v);
    long liveDuration();
    QString fSession;

protected:
    HttpHeader fHttpHeader;
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) = 0;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) = 0;
    bool setResponse(int responseCode, const QString &data);
    bool setInternalServerError(const QString &msg);
    bool setForbiddenError(const QString &msg);
    bool setDataValidationError(const QString &msg);

private:
    bool fIdle;
    QDateTime fCreated;

signals:
    void sendData(int code, const QString &, const QString &, const QVariant &);

};

#endif // REQUESTHANDLER_H
