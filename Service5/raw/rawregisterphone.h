#ifndef RAWREGISTERPHONE_H
#define RAWREGISTERPHONE_H

#include "raw.h"

class RawRegisterPhone : public Raw
{
    Q_OBJECT
public:
    RawRegisterPhone(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;

private slots:
    void fcmFinished();

};

#endif // RAWREGISTERPHONE_H
