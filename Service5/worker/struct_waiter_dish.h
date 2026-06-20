#pragma once

#include "c5jsonparser.h"
#include <QStringList>
#include <QJsonDocument>

struct WaiterDish {
    QString id;
    QString header;
    int state;
    int type;
    QString parent;
    int dishId;
    int store;
    QString dishName;
    QString unitName;
    double qty;
    double price;
    int row;
    /** Только для PACKAGE (тип 5): Σ(qty×price) состава − qty×price строки пакета; считается на клиенте. */
    double packageNominalDelta = 0;
    QString emarks() const { return data.value("f_emarks").toString(); }
    QJsonObject data;
    QString nameLower;
    QStringList words;

    bool isService() const { return data.value("f_is_service").toBool(); }

    bool isPiece() const { return data.value("f_is_piece").toBool(); }

    QString barcode() const { return data.value("f_barcode").toString(); }

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
        double delta = 0;
        if (countService()) {
            delta += serviceFactor();
        }
        if (countDiscount()) {
            delta -= qAbs(discountFactor());
        }
        total += total * delta;
        return total;
    }

    /** Line amount for display / client totals (order-level service/discount factors). */
    double lineAmount(bool isPreorder, bool includeUnprinted,
                      double orderServiceFactor, double orderDiscountFactor) const
    {
        if (data.value(QStringLiteral("f_complimentary")).toBool()) {
            return 0;
        }
        if (!isPreorder && !includeUnprinted && !isPrinted()) {
            return 0;
        }

        double priceModificator = 0;

        if (countService()) {
            priceModificator += orderServiceFactor;
        }
        if (countDiscount()) {
            priceModificator -= qAbs(orderDiscountFactor);
        }

        return qty * price * (1.0 + priceModificator);
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

    QString adgtCode() { return data["f_adgt"].toString(); }
    QString translated() const
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
    QString fiscalName() const { return data.value("f_fiscal_name").toString(); }
    QString adgt() const { return data.value("f_adgt").toString(); }
    double serviceFactor() const { return data.value("f_service_factor").toDouble(); }
    double discountFactor() const
    {
        return data["f_discount_factor"].toDouble();
    }
    bool isHourlyPayment() const { return data.value("f_hourly_payment").toBool(); }
    bool isPlaying() const { return !data.value("f_stopped").toBool(); }
    QString hourlyRule() const { return data.value("f_hourly_rule").toString(); }
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
        j["f_emarks"] = emarks();
        j[QStringLiteral("f_data")] = data;
        if(data.contains(QStringLiteral("f_kitchen_status"))) {
            j[QStringLiteral("f_process_status")] = data.value(QStringLiteral("f_kitchen_status"));
        }
        if(data.contains(QStringLiteral("f_goods_process"))) {
            j[QStringLiteral("f_goods_process")] = data.value(QStringLiteral("f_goods_process"));
        }
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
        wd.parent = jo.value("f_parent").toString();
        wd.state = jo["f_state"].toInt();
        wd.store = jo["f_store"].toInt();
        wd.dishId = jo["f_dish"].toInt();
        wd.dishName = jo["f_dish_name"].toString();
        wd.unitName = jo["f_unit_name"].toString();
        wd.row = jo["f_row"].toInt();
        wd.qty = jo["f_qty"].toDouble();
        wd.price = jo["f_price"].toDouble();
        wd.data = {};

        if(jo.contains(QStringLiteral("f_data"))) {
            const QJsonValue fd = jo.value(QStringLiteral("f_data"));
            if(fd.isObject()) {
                wd.data = fd.toObject();
            } else if(fd.isString()) {
                QJsonParseError err;
                const QByteArray raw = fd.toString().toUtf8();
                const QJsonDocument doc = QJsonDocument::fromJson(raw, &err);

                if(err.error == QJsonParseError::NoError && doc.isObject()) {
                    wd.data = doc.object();
                } else {
                    qWarning() << "f_data parse error:" << err.errorString()
                               << "raw:" << raw;
                }
            }
        }

        if(jo.contains(QStringLiteral("f_process_status"))) {
            wd.data.insert(QStringLiteral("f_kitchen_status"), jo.value(QStringLiteral("f_process_status")).toInt(0));
        }

        const QJsonValue processRaw = jo.value(QStringLiteral("f_process_data"));
        if(processRaw.isObject()) {
            wd.data.insert(QStringLiteral("f_goods_process"), processRaw.toObject());
        } else if(processRaw.isString() && !processRaw.toString().trimmed().isEmpty()) {
            QJsonParseError err;
            const QJsonDocument doc = QJsonDocument::fromJson(processRaw.toString().toUtf8(), &err);
            if(err.error == QJsonParseError::NoError && doc.isObject()) {
                wd.data.insert(QStringLiteral("f_goods_process"), doc.object());
            }
        }

        wd.data["f_fiscal_department"] = jo["f_fiscal_department"];
        wd.data["f_adgt"] = jo["f_adgt"];

        return wd;
    }

    static WaiterDish* pointerFromJson(const QJsonObject &jo)
    {
        return new WaiterDish(fromJson(jo));
    }
};
