#pragma once

#include "c5jsonparser.h"
#include "struct_parent.h"
#include <QStringList>

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
        return hi;
    }
};

extern WorkstationItem mWorkStation;
