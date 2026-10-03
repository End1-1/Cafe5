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
    bool externalPos = false;
    bool idramExternalPos = false;
    bool simpleFiscal = false;
    QString externalPosString() const { return externalPos ? "true" : "false"; }
    QString idramExternalPosString() const { return idramExternalPos ? "true" : "false"; }
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
        auto flagOn = [](const QJsonValue &v) {
            if (v.isNull() || v.isUndefined()) {
                return false;
            }
            if (v.isBool()) {
                return v.toBool();
            }
            if (v.isDouble()) {
                return qRound(v.toDouble()) == 1;
            }
            const QString s = v.toString().trimmed();
            return v.toInt() == 1 || s == QLatin1String("1")
                || s.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
        };
        fm.externalPos = flagOn(jo.value("f_external_pos"));
        fm.idramExternalPos = flagOn(jo.value("f_idram_ext_pos"));
        fm.simpleFiscal = flagOn(jo.value("f_simple_fiscal"));
        return fm;
    }
};

extern QList<FiscalMachine> fiscalMachines;
extern FiscalMachine getFiscalMachine(int id);
