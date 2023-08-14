#ifndef RAWREGISTERPHONE_H
#define RAWREGISTERPHONE_H

#include "raw.h"

class RawRegisterPhone : public Raw
{
    Q_OBJECT
public:
    RawRegisterPhone(SslSocket *s);
    ~RawRegisterPhone();

public slots:
    virtual int run(const QByteArray &d) override;

signals:
    void firebaseMessage(const QString &, const QString &);

};

#endif // RAWREGISTERPHONE_H
