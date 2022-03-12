#ifndef RAWREGISTERPHONE_H
#define RAWREGISTERPHONE_H

#include "raw.h"

class RawRegisterPhone : public Raw
{
    Q_OBJECT
public:
    RawRegisterPhone(SslSocket *s, const QByteArray &d);
    ~RawRegisterPhone();

public slots:
    virtual void run() override;

signals:
    void registerPhone(SslSocket*, const QString &);

};

#endif // RAWREGISTERPHONE_H
