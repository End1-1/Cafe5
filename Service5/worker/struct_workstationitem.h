#pragma once

#include <QStringList>
#include "c5jsonparser.h"
#include "struct_fiscal_machine.h"
#include "struct_parent.h"

struct WorkstationItem : public ParentItem {
    int id = 0;
    int type = 0;
    QString stationAccount;
    QString name;
    QString nameLower;

    virtual void validate() override
    {
        Q_ASSERT_X(
            data.contains("f_cashbox_id"),
            "WorkstationItem",
            "Data not contains cashbox_id key"
        );
    }

    int cashboxId() const
    {
        return data.value("f_cashbox_id").toInt();
    }
    int defaultHallId() const
    {
        return data.value("f_default_hall_id").toInt();
    }
    const QString defaultPrinter() const
    {
        QString printerName = data.value("f_default_printer").toString();
        return printerName.isEmpty() ? "local" : printerName;
    }
    const QString presentCardPattern() const
    {
        return data.value("f_present_card_pattern").toString();
    }
    const QString accumulateCardPatter() const
    {
        return data.value("f_accumulate_card_pattern").toString();
    }
    const QString discountCardPattern() const
    {
        return data.value("f_discount_card_pattern").toString();
    }
    const bool shopShowAll() const
    {
        if (!data.contains("f_showall")) {
            return true;
        }
        return data.value("f_showall").toBool();
    }
    const int fiscalMachineId() const { return data.value("f_fiscal_machine_id").toInt(); }
    const int quickDebtPartnerId() const { return data.value("f_quick_debt_partner_id").toInt(); }
};

template<>
struct JsonParser<WorkstationItem> {
    static WorkstationItem fromJson(const QJsonObject &jo)
    {
        WorkstationItem hi;
        hi.id = jo["f_id"].toInt();
        hi.type = jo["f_type"].toInt();
        hi.stationAccount = jo["f_station_account"].toString();
        hi.name = jo["f_name"].toString();
        hi.parseData(jo, "f_config");
        hi.validate();

        QJsonArray jfm = jo.value("fiscal").toArray();
        for (int i = 0; i < jfm.size(); i++) {
            const QJsonObject &jf = jfm.at(i).toObject();
            FiscalMachine fm = JsonParser<FiscalMachine>::fromJson(jf);
            fiscalMachines.append(fm);
        }
        return hi;
    }
};

extern WorkstationItem mWorkStation;
