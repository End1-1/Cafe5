#pragma once

#include <QStringList>
#include <QJsonDocument>
#include <QJsonParseError>
#include "c5jsonparser.h"
#include "c5utils.h"
#include "dict_dish_state.h"
#include "dict_goods_types.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include "struct_waiter_dish.h"

struct WaiterOrderCalculatedAmounts {
    double subtotal = 0;
    double serviceAmount = 0;
    double discountAmount = 0;
    double totalDue = 0;
};

struct WaiterOrder {
    QString id;
    int state = 0;
    double totalDue = 0;
    QString receiptNumber;
    int table = 0;
    int cashierId;
    int cashSessionId = 0;
    QString cashierName;
    int staffId;
    QString staffName;
    QString hallName;
    QString tableName;
    QJsonObject data;
    QList<WaiterDish> dishes;
    QList<WaiterDish> precheckDishes;
    QJsonArray calcQueue;
    QString nameLower;
    QStringList words;
    QJsonObject rawBody;
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

            if (d.type == GOODS_TYPE_GOODS || d.type == GOODS_TYPE_DISH || d.type == GOODS_TYPE_PACKAGE) {
                if (!d.isPrinted()) {
                    return false;
                }
            }
        }

        return true;
    }

    bool isPrecheckPrinted()
    {
        return data["f_precheck"].toInt() > 0;
    }
    QJsonObject fiscal() const { return data.value("f_fiscal").toObject(); }
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
    double prepaidAmount() const { return data.value("f_deposit_prepaid").toVariant().toDouble(); }

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
        return data.value(name).toDouble();
    }
    int printCount() const
    {
        return data["f_print_count"].toInt();
    }
    double paidCash() const { return data["f_amount_cash"].toDouble(); }
    double paidCard() const { return data["f_amount_card"].toDouble(); }
    double paidIdram() const { return data["f_amount_idram"].toDouble(); }
    double paidTelcell() const { return data["f_amount_telcell"].toDouble(); }
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
    QDateTime dateOpen()const
    {
        return QDateTime::fromString(data.value("f_date_open").toString() + " " + data.value("f_time_open").toString(), FORMAT_DATETIME_TO_STR_MYSQL);
    }
    QDateTime dateClose()const
    {
        return QDateTime::fromString(data.value("f_date_close").toString() + " " + data.value("f_time_close").toString(), FORMAT_DATETIME_TO_STR_MYSQL);
    }
    QJsonValue dataValue(const QString &key) const
    {
        return data.value(key);
    }
    int normalDishesCount() const
    {
        int c = 0;
        for (auto const &d : dishes) {
            if (d.state == 1) {
                c++;
            }
        }
        return c;
    }

    /** Client-side bill amounts (same rules as waiter order.php CountAmounts). */
    WaiterOrderCalculatedAmounts calculatedAmounts(bool includeUnprinted = false) const
    {
        const bool isPreorder = (state == ORDER_STATE_PREORDER);
        const double orderServiceFactor = serviceFactor();
        const double orderDiscountFactor = qAbs(discountFactor());

        WaiterOrderCalculatedAmounts amounts;

        for (const WaiterDish &d : dishes) {
            if (d.state != DISH_STATE_OK) {
                continue;
            }
            if (d.data.value(QStringLiteral("f_complimentary")).toBool()) {
                continue;
            }

            if (!d.parent.isEmpty()) {
                bool packageChild = false;
                for (const WaiterDish &p : dishes) {
                    if (p.id == d.parent && p.type == GOODS_TYPE_PACKAGE) {
                        packageChild = true;
                        break;
                    }
                }
                if (packageChild) {
                    continue;
                }
            }

            if (!isPreorder && !includeUnprinted && !d.isPrinted()) {
                continue;
            }

            amounts.subtotal += d.price * d.qty;

            if (d.countService()) {
                amounts.serviceAmount += d.price * orderServiceFactor * d.qty;
            }
            if (d.countDiscount()) {
                amounts.discountAmount += d.price * orderDiscountFactor * d.qty;
            }

            amounts.totalDue += d.lineAmount(isPreorder, includeUnprinted,
                                             orderServiceFactor, orderDiscountFactor);
        }

        return amounts;
    }

    double calculatedTotalDue(bool includeUnprinted = false) const
    {
        return calculatedAmounts(includeUnprinted).totalDue;
    }
};

template<>
struct JsonParser<WaiterOrder> {
    static WaiterOrder fromJson(const QJsonObject &jo)
    {
        WaiterOrder wo;
        wo.rawBody = jo;
        wo.id = jo["f_id"].toString();
        wo.cashSessionId = jo["f_cash_session_id"].toInt();
        wo.state = jo["f_state"].toInt();
        wo.receiptNumber = jo["f_prefix"].toString();
        wo.table = jo["f_table"].toInt();
        wo.cashierId = jo["f_cashier"].toInt();
        wo.cashierName = jo["f_cashier_name"].toString();
        wo.staffId = jo["f_staff"].toInt();
        wo.staffName = jo["f_staff_name"].toString();
        wo.hallName = jo["f_hall_name"].toString();
        wo.tableName = jo["f_table_name"].toString();
        wo.totalDue = jo["f_amounttotal"].toDouble();
        {
            const QJsonValue fd = jo.value(QStringLiteral("f_data"));

            if(fd.isObject()) {
                wo.data = fd.toObject();
            } else if(fd.isString()) {
                QJsonParseError pe{};
                const QJsonDocument doc = QJsonDocument::fromJson(fd.toString().toUtf8(), &pe);
                wo.data = doc.isObject() ? doc.object() : QJsonObject{};
            } else {
                wo.data = QJsonObject{};
            }
        }
        const QJsonArray dishes = jo["dishes"].toArray();

        for(const QJsonValue &v : dishes) {
            wo.dishes.append(JsonParser<WaiterDish>::fromJson(v.toObject()));
        }

        const QJsonArray precheckdishes = jo["precheck_dishes"].toArray();

        for(const QJsonValue &v : precheckdishes) {
            wo.precheckDishes.append(JsonParser<WaiterDish>::fromJson(v.toObject()));
        }

        wo.calcQueue = jo.value(QStringLiteral("calc_queue")).toArray();

        return wo;
    }

    static WaiterOrder* pointerFromJson(const QJsonObject &jo)
    {
        return new WaiterOrder(fromJson(jo));
    }
};
