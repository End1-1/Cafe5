#ifndef RAWPLUGIN_H
#define RAWPLUGIN_H

#include "raw.h"

class RawPlugin : public Raw
{
public:
    RawPlugin(SslSocket *s);

public slots:
    virtual int run(const QByteArray &d) override;

};

#endif // RAWPLUGIN_H
