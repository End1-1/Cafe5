#include "notfound.h"
#include "jsonhandler.h"

NotFound::NotFound(const QString &route) :
    RequestHandler()
{
    JsonHandler jh;
    jh["message"] = QString("%1 not exists on this server").arg(route);
    fHttpHeader.setResponseCode(HTTP_NOT_FOUND);
    fHttpHeader.setContentLength(jh.length());
    fResponse.append(fHttpHeader.toString());
    fResponse.append(jh.toString());
}

NotFound::~NotFound()
{
    __debug_log("~NotFound");
}

bool NotFound::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    Q_UNUSED(dataMap);
    return true;
}

bool NotFound::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    Q_UNUSED(dataMap);
    return true;
}
