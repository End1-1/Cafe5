#ifndef CASHBOXCONFIG_H
#define CASHBOXCONFIG_H

#include "config.h"

class CashboxConfig : public Config
{
public:
    CashboxConfig(const QStringList &dbParams, int id);
    virtual void readValues() override;

    int cash1;
    int cash2;
    int cash3;
    int cash4;
    int cash5;
    int cash6;
    int cash7;

    QString cash1Name;
    QString cash2Name;
    QString cash3Name;
    QString cash4Name;
    QString cash5Name;
    QString cash6Name;
    QString cash7Name;

    bool cash1Enabled;
    bool cash2Enabled;
    bool cash3Enabled;
    bool cash4Enabled;
    bool cash5Enabled;
    bool cash6Enabled;
    bool cash7Enabled;

    bool cash6Hotel;
    bool cash6Debt;
    bool cash6Selfcost;
    bool cash6Present;
};

#endif // CASHBOXCONFIG_H
