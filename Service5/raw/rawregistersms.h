#ifndef RAWREGISTERSMS_H
#define RAWREGISTERSMS_H

#include "raw.h"

class RawRegisterSMS : public Raw
{
    Q_OBJECT
public:
    RawRegisterSMS(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;

signals:
    void registerSms(SslSocket*, const QString &);
};

#endif // RAWREGISTERSMS_H
