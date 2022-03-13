#ifndef RAWREGISTERSMS_H
#define RAWREGISTERSMS_H

#include "raw.h"

class RawRegisterSMS : public Raw
{
    Q_OBJECT
public:
    RawRegisterSMS(SslSocket *s);
    ~RawRegisterSMS();

public slots:
    virtual void run(const QByteArray &d) override;
};

#endif // RAWREGISTERSMS_H
