#ifndef BREEZECONFIG_H
#define BREEZECONFIG_H

#include "configs.h"

class BreezeConfig : public Configs
{
public:
    QString ipAddress;
    int port;
    QString apiKey;
    QString username;
    QString password;
    QString session;
    QString dealtype;
    BreezeConfig(const QStringList &dbParams, int id);
    virtual void readValues() override;
};

#endif // BREEZECONFIG_H
