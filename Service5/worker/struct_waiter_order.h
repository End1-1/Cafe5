#pragma once

#include "c5jsonparser.h"
#include "struct_waiter_dish.h"
#include "dict_dish_state.h"
#include "dict_payment_type.h"
#include <QStringList>

struct WaiterOrder {
    QString id;
    int state = 0;
    double totalDue = 0;
    QString receiptNumber;
    int table = 0;
    QString hallName;
    QString tableName;
    QJsonObject data;
    QList<WaiterDish> dishes;
    QList<WaiterDish> precheckDishes;
    QString nameLower;
    QStringList words;
    bool isEmpty()
    {
        for(auto d : dishes) {
            if(d.state == DISH_STATE_OK) {
                return false;
            }
        }

        return true;
    }
    bool isReadyForPrecheck()
    {
        for(auto d : dishes) {
            if(d.state != DISH_STATE_OK) {
                continue;
            }

            if(!d.isPrinted()) {
                return false;
            }
        }

        return true;
    }

    bool isPrecheckPrinted()
    {
        return data["f_precheck"].toInt() > 0;
    }
    QJsonObject fiscal() const
    {
        return data["f_fiscal"].toObject();
    }
    QString comment() const
    {
        return data["f_comment"].toString();
    }
    double subTotal() const
    {
        return data["f_sub_total"].toDouble();
    }
    double serviceFactor() const
    {
        return data["f_service_factor"].toDouble();
    }
    double serviceAmount() const
    {
        return data["f_service_amount"].toDouble();
    }
    double discountFactor() const
    {
        return data["f_discount_factor"].toDouble();
    }
    double discountAmount() const
    {
        return data["f_discount_amount"].toDouble();
    }
    double prepaidAmount() const
    {
        return data["f_prepaid_amount"].toDouble();
    }

    double amountPaid() const
    {
        return data["f_amount_paid"].toDouble();
    }
    double amountChange() const
    {
        return data["f_amount_change"].toDouble();
    }
    double payment(const QString &name) const
    {
        return data[name].toDouble();
    }
    int printCount() const
    {
        return data["f_print_count"].toInt();
    }
    double paidCash() const
    {
        return data["f_paid_cash"].toDouble();
    }
    double paidCard() const
    {
        return data["f_paid_card"].toDouble();
    }
    double paidPrepaid() const
    {
        return data["f_paid_prepaid"].toDouble();
    }
    bool paymentCompleted()
    {
        double totalPaid = 0;

        for(auto pt : payment_types) {
            totalPaid += payment(payment_fields[pt]);
        }

        return totalPaid >= totalDue;
    }
};

template<>
struct JsonParser<WaiterOrder> {
    static WaiterOrder fromJson(const QJsonObject &jo)
    {
        WaiterOrder wo;
        wo.id = jo["f_id"].toString();
        wo.state = jo["f_state"].toInt();
        wo.receiptNumber = jo["f_prefix"].toString();
        wo.table = jo["f_table"].toInt();
        wo.hallName = jo["f_hall_name"].toString();
        wo.tableName = jo["f_table_name"].toString();
        wo.totalDue = jo["f_amounttotal"].toDouble();
        wo.data = QJsonDocument::fromJson(jo["f_data"].toString().toUtf8()).object();
        const QJsonArray dishes = jo["dishes"].toArray();

        for(const QJsonValue &v : dishes) {
            wo.dishes.append(JsonParser<WaiterDish>::fromJson(v.toObject()));
        }

        const QJsonArray precheckdishes = jo["precheck_dishes"].toArray();

        for(const QJsonValue &v : precheckdishes) {
            wo.precheckDishes.append(JsonParser<WaiterDish>::fromJson(v.toObject()));
        }

        return wo;
    }

    static WaiterOrder* pointerFromJson(const QJsonObject &jo)
    {
        return new WaiterOrder(fromJson(jo));
    }
};
