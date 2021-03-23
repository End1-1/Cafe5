#ifndef HTTPHEADER_H
#define HTTPHEADER_H

#include <QString>

#define HTTP_OK 200
#define HTTP_SEE_OTHER 303
#define HTTP_NOT_FOUND 404
#define HTTP_FORBIDDEN 403
#define HTTP_DATA_VALIDATION_ERROR 422
#define HTTP_INTERNAL_SERVER_ERROR 500

class HttpHeader
{
public:
    explicit HttpHeader();
    void reset();
    void setResponseCode(int responseCode);
    void setContentLength(int size);
    void setLocation(const QString &location);
    QString toString() const;

private:
    int fResponseCode;
    int fContentLength;
    QString fLocation;
    QString responseMessage() const;
};

#endif // HTTPHEADER_H
