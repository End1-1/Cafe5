#pragma once

#include "c5jsonparser.h"
#include <QStringList>
#include <QJsonDocument>

struct WaiterDish {
    QString id;
    QString header;
    int state;
    int type;
    int parent;
    int dishId;
    int store;
    QString dishName;
    double qty;
    double price;
    int row;
    QString emarks;
    QJsonObject data;
    QString nameLower;
    QStringList words;

    QString comment()const
    {
        return data["f_comment"].toString();
    }
    QString removeReason() const
    {
        return data["f_remove_reason"].toString();
    }
    QString fromTable() const
    {
        return data.value("f_from_table").toString();
    };
    bool isPrinted() const
    {
        return data.value("f_printed").toBool();
    }
    double total(bool isPreorder) const
    {
        if(!isPrinted() && !isPreorder) {
            return 0;
        }

        double total = qty * price;
        return total;
    }
    QString appendedTime()
    {
        return data["f_append_time"].toString();
    }

    QString printedTime()
    {
        return data["f_print_time"].toString();
    }
    QString removedTime()
    {
        return data["f_remove_time"].toString();
    }
    bool isHourlyPayment()
    {
        return data["f_hourly"].toBool();
    }
    QString adgtCode()
    {
        return data["f_adgt_code"].toString();
    }
    QString translated()
    {
        return dishName;
    }
    bool countService() const
    {
        return data.value("f_count_service").toBool();
    }
    bool countDiscount() const
    {
        return data.value("f_count_discount").toBool();
    }
    bool complimentary()
    {
        return data["f_complimentary"].toBool();
    }
    QString printer1() const
    {
        return data["f_print1"].toString();
    }
    QString printer2() const
    {
        return data["f_print2"].toString();
    }
    int fiscalDepartment()const
    {
        return data["f_fiscal_department"].toInt();
    }
    double discountFactor() const
    {
        return data["f_discount_factor"].toDouble();
    }
    QJsonValue dataValue(const QString &key) const
    {
        return data.value(key);
    }
    QJsonObject toJson() const
    {
        QJsonObject j;
        j["f_id"] = id;
        j["f_header"] = header;
        j["f_state"] = state;
        j["f_type"] = type;
        j["f_parent"] = parent;
        j["dishId"] = dishId;
        j["f_store"] = store;
        j["dishName"] = dishName;
        j["f_qty"] = qty;
        j["f_price"] = price;
        j["f_row"] = row;
        j["f_emarks"] = emarks;
        j["f_data"] = data;
        return j;
    }
};

template<>
struct JsonParser<WaiterDish> {
    static WaiterDish fromJson(const QJsonObject &jo)
    {
        WaiterDish wd;
        wd.id = jo["f_id"].toString();
        wd.header = jo["f_header"].toString();
        wd.type = jo["f_type"].toInt();
        wd.parent = jo["f_parent"].toInt();
        wd.state = jo["f_state"].toInt();
        wd.store = jo["f_store"].toInt();
        wd.dishId = jo["f_dish"].toInt();
        wd.dishName = jo["f_dish_name"].toString();
        wd.row = jo["f_row"].toInt();
        wd.qty = jo["f_qty"].toDouble();
        wd.price = jo["f_price"].toDouble();
        wd.emarks = jo["f_emarks"].toString();
        wd.data = {};

        if(jo.contains("f_data") && jo["f_data"].isString()) {
            QJsonParseError err;
            const QByteArray raw = jo["f_data"].toString().toUtf8();
            QJsonDocument doc = QJsonDocument::fromJson(raw, &err);

            if(err.error == QJsonParseError::NoError && doc.isObject()) {
                wd.data = doc.object();
            } else {
                qWarning() << "f_data parse error:" << err.errorString()
                           << "raw:" << raw;
            }
        }

        return wd;
    }

    static WaiterDish* pointerFromJson(const QJsonObject &jo)
    {
        return new WaiterDish(fromJson(jo));
    }
};
