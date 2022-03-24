#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "socketdata.h"
#include "socketdata.h"
#include "httpheader.h"
#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QHash>

class RequestHandler : public QObject
{
    Q_OBJECT

public:
    explicit RequestHandler(QByteArray &out);
    virtual ~RequestHandler();
    ContentType fContentType;
    HttpHeader fHttpHeader;
    QString fSession;
    bool setResponse(int responseCode, const QString &data);
    bool setInternalServerError(const QString &msg);
    bool setForbiddenError(const QString &msg);
    bool setDataValidationError(const QString &msg);

protected:
    QByteArray &fResponse;
};

#endif // REQUESTHANDLER_H
