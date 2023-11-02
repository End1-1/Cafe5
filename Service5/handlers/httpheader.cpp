#include "httpheader.h"

HttpHeader::HttpHeader()
{
    fResponseCode = HTTP_OK;
    fContentLength = 0;
}

void HttpHeader::reset()
{
    fResponseCode = HTTP_OK;
    fContentLength = 0;
    fLocation.clear();
}

void HttpHeader::setResponseCode(int responseCode)
{
    fResponseCode = responseCode;
}

QString HttpHeader::toString() const
{
    QString str;
    str.append("HTTP/1.1 ");
    str.append(QString::number(fResponseCode));
    str.append(" ");
    str.append(responseMessage());
    str.append("\r\n");
    str.append(QString("Content-Type: application/json;charset=UTF-8"));
    str.append("\r\n");
    str.append(QString("Access-Control-Allow-Origin: *"));
    str.append("\r\n");
    str.append(QString("Content-Length: %1").arg(fContentLength));
    if (!fLocation.isEmpty()) {
        str.append("\r\n");
        str.append(QString("Location: %1").arg(fLocation));
    }
    str.append("\r\n\r\n");
    return str;
}

QString HttpHeader::responseMessage() const
{
    switch (fResponseCode) {
    case HTTP_OK:
        return "OK";
    case HTTP_SEE_OTHER:
        return "See other";
    case HTTP_NOT_FOUND:
        return "Not found";
    case HTTP_FORBIDDEN:
        return "Forbidden";
    case HTTP_DATA_VALIDATION_ERROR:
        return "Data validation error";
    case HTTP_INTERNAL_SERVER_ERROR:
        return "Internal server error";
    default:
        return "I dont know";
    }
}

void HttpHeader::setContentLength(int contentLength)
{
    fContentLength = contentLength;
}

void HttpHeader::setLocation(const QString &location)
{
    fLocation = location;
}
