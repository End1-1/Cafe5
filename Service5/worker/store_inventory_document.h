#pragma once

#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include "c5jsonparser.h"
#include "c5utils.h"
#include "struct_parent.h"

struct StoreInventoryUser
{
    QString uuid;
    int item_id = 0;
    QString item_name;
    QString unit_name;
    QString group_name;
    double qty_user = 0;
    double qty_sys = 0;
    double qty_diff = 0;
    double price = 0;
    int row;
    QJsonObject toJson()
    {
        return {{"id", uuid}, {"item_id", item_id}, {"qty_user", qty_user}, {"qty_sys", qty_sys}, {"price", price}, {"row", row}};
    }
};

struct StoreInventoryDocument : public ParentItem
{
    QString uuid;
    QString user_id;
    QString date;
    int create_user = 0;
    int store = 0;
    QString store_name;

    QList<StoreInventoryUser> items;

    QString partnerName() const { return data.value("f_partner_name").toString(); }
    QString comment() const { return data.value("f_comment").toString(); }

    QJsonObject toJson()
    {
        QJsonArray jitems;
        for (auto item : std::as_const(items)) {
            jitems.append(item.toJson());
        }
        return {{"doc_uuid", uuid},
                {"f_date", date},
                {"f_store", store == 0 ? QJsonValue::Null : store},
                {"doc_create_user", create_user},
                {"doc_data", data},
                {"items", jitems}};
    }
};

template<>
struct JsonParser<StoreInventoryUser>
{
    static StoreInventoryUser fromJson(const QJsonObject &jo)
    {
        StoreInventoryUser sid;
        sid.uuid = jo.value("f_id").toString();
        sid.group_name = jo.value("f_group_name").toString();
        sid.item_id = jo.value("f_item_id").toInt();
        sid.item_name = jo.value("f_item_name").toString();
        sid.unit_name = jo.value("f_unit_name").toString();
        sid.qty_user = str_float(jo.value("f_qty_user").toString());
        sid.qty_sys = str_float(jo.value("f_qty_sys").toString());
        sid.qty_diff = str_float(jo.value("f_qty_diff").toString());
        sid.price = str_float(jo.value("f_price").toString());

        return sid;
    }

    static StoreInventoryUser *pointerFromJson(const QJsonObject &jo) { return new StoreInventoryUser(fromJson(jo)); }
};

template<>
struct JsonParser<StoreInventoryDocument>
{
    static StoreInventoryDocument fromJson(const QJsonObject &jo)
    {
        StoreInventoryDocument sid;
        sid.uuid = jo.value("f_id").toString();
        sid.date = jo.value("f_date").toString();
        sid.store = jo.value("f_store").toInt();
        sid.store_name = jo.value("f_store_name").toString();

        sid.parseData(jo);
        QJsonArray ji = jo.value("items").toArray();
        for (int i = 0; i < ji.size(); i++) {
            StoreInventoryUser su = JsonParser<StoreInventoryUser>::fromJson(ji.at(i).toObject());
            sid.items.append(su);
        }
        return sid;
    }

    static StoreInventoryDocument *pointerFromJson(const QJsonObject &jo) { return new StoreInventoryDocument(fromJson(jo)); }
};
