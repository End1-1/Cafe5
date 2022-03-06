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
};

#endif // RAWREGISTERSMS_H
