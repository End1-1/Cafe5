#ifndef BREEZECONFIG_H
#define BREEZECONFIG_H

#include "config.h"


class BreezeConfig : public Config
{
public:
    QString ipAddress;
    int port;
    QString apiKey;
    QString username;
    QString password;
    QString session;
    BreezeConfig(const QStringList &dbParams, int id);
    virtual void readValues() override;
};

#endif // BREEZECONFIG_H
