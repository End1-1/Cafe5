#include "requesthandler.h"
#include "database.h"
#include "logwriter.h"
#include "databaseconnectionmanager.h"
#include <QElapsedTimer>

RequestHandler::RequestHandler(QByteArray &out) :
    fResponse(out)
{
    fContentType = ContentType::MultipartFormData;
}

RequestHandler::~RequestHandler()
{
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
