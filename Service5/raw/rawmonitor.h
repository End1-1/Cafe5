#ifndef RAWMONITOR_H
#define RAWMONITOR_H

#include "raw.h"

class RawMonitor : public Raw
{
    Q_OBJECT
public:
    RawMonitor(SslSocket *s);
    ~RawMonitor();

public slots:
    virtual void run(const QByteArray &d) override;

};

#endif // RAWMONITOR_H
