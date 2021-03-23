#include "forbidden.h"
#include "jsonhandler.h"

Forbidden::Forbidden() :
    RequestHandler()
{

}

void Forbidden::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    Q_UNUSED(dataMap);
    JsonHandler jh;
    jh["message"] = "Forbidden";
    fHttpHeader.setResponseCode(HTTP_FORBIDDEN);
    fHttpHeader.setContentLength(jh.length());
    fResponse.append(fHttpHeader.toString());
    fResponse.append(jh.toString());
}

bool Forbidden::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    Q_UNUSED(dataMap);
    return true;
}
