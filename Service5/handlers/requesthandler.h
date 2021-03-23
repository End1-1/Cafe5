#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "socketdata.h"
#include "debug.h"
#include "httpheader.h"
#include <QByteArray>
#include <QHash>

class RequestHandler
{
public:
    explicit RequestHandler();
    virtual ~RequestHandler();
    static RequestHandler *route(const QString &remoteHost, const QString &r, const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    QByteArray fResponse;
    bool idle();
    void setIdle(bool v);

protected:
    HttpHeader fHttpHeader;
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) = 0;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) = 0;
    void setResponse(int responseCode, const QString &data);

private:
    bool fIdle;

};

#endif // REQUESTHANDLER_H
