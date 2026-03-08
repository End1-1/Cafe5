#pragma once

#include "c5jsonparser.h"
#include "struct_parent.h"

struct FiscalMachine : public ParentItem
{
    int id = 0;
    QString name;
    QString ip;
    QString machinePassword;
    int port = 0;
    int defaultDept = 0;
    QString opPin;
    QString opPassword;
    bool externalPos;
    QString externalPosString() const { return externalPos ? "true" : "false"; }
};

template<>
struct JsonParser<FiscalMachine>
{
    static FiscalMachine fromJson(const QJsonObject &jo)
    {
        FiscalMachine fm;
        fm.id = jo.value("f_id").toInt();
        fm.name = jo.value("f_name").toString();
        fm.ip = jo.value("f_ip").toString();
        fm.machinePassword = jo.value("f_password").toString();
        fm.port = jo.value("f_port").toInt();
        fm.defaultDept = jo.value("f_default_dept").toInt();
        fm.opPin = jo.value("f_op_pin").toString();
        fm.opPassword = jo.value("f_op_pass").toString();
        fm.externalPos = jo.value("f_external_pos").toInt() == 1;
        return fm;
    }
};

extern QList<FiscalMachine> fiscalMachines;
extern FiscalMachine getFiscalMachine(int id);
