#ifndef RAWMONITOR_H
#define RAWMONITOR_H

#include "raw.h"

class RawMonitor : public Raw
{
    Q_OBJECT
public:
    RawMonitor(SslSocket *s, const QByteArray &d);
    ~RawMonitor();

public slots:
    virtual void run() override;

signals:
    void registerMonitor(SslSocket *);
};

#endif // RAWMONITOR_H
