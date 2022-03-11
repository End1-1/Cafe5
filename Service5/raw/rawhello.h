#ifndef RAWHELLO_H
#define RAWHELLO_H

#include "raw.h"

class RawHello : public Raw
{
    Q_OBJECT

public:
    RawHello(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run();

signals:
    void registerFirebaseToken(SslSocket*, const QString &);
};

#endif // RAWHELLO_H
