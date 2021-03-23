#ifndef NOTFOUND_H
#define NOTFOUND_H

#include "requesthandler.h"

class NotFound : public RequestHandler
{
public:
    NotFound(const QString &route);
    ~NotFound();

protected:
    virtual void handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
};

#endif // NOTFOUND_H
