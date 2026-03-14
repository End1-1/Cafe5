#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include "c5jsonparser.h"
#include "struct_parent.h"

struct StoreUser
{
    QString uuid;
    int item_id = 0;
    QString item_name;
    QString unit_name;
    QString adgt;
    double qty = 0;
    double price = 0;
    double total() { return qty * price; }
    QString expire_date;
    QString comment;
    int row;
    QJsonObject toJson()
    {
        return {{"id", uuid},
                {"item_id", item_id},
                {"qty", qty},
                {"price", price},
                {"expire_date", expire_date},
                {"comment", comment},
                {"row", row}};
    }
};

struct StoreInputDocument : public ParentItem
{
    QString uuid;
    QString user_id;
    QString date;
    int status = 0;
    int type = 0;
    int create_user = 0;
    int store_in = 0;
    QString store_in_name;
    int store_out = 0;
    QString store_out_name;
    int version = 0;
    double sum = 0;
    int partner = 0;
    QList<StoreUser> items;

    QString partnerName() const { return data.value("f_partner_name").toString(); }
    QString comment() const { return data.value("comment").toString(); }

    QJsonObject toJson()
    {
        QJsonArray jitems;
        for (auto item : std::as_const(items)) {
            jitems.append(item.toJson());
        }
        return {{"doc_uuid", uuid},
                {"doc_user_id", user_id},
                {"doc_date", date},
                {"doc_status", status},
                {"doc_type", type},
                {"doc_store_in", store_in == 0 ? QJsonValue::Null : store_in},
                {"doc_store_out", store_out == 0 ? QJsonValue::Null : store_out},
                {"doc_sum", sum},
                {"doc_partner", partner == 0 ? QJsonValue::Null : partner},
                {"doc_create_user", create_user},
                {"doc_version", version},
                {"doc_data", data},
                {"items", jitems}};
    }
};

template<>
struct JsonParser<StoreUser>
{
    static StoreUser fromJson(const QJsonObject &jo)
    {
        StoreUser sid;
        sid.uuid = jo.value("f_id").toString();
        sid.adgt = jo.value("f_adg").toString();
        sid.item_id = jo.value("f_item_id").toInt();
        sid.item_name = jo.value("f_item_name").toString();
        sid.unit_name = jo.value("f_unit_name").toString();
        sid.expire_date = jo.value("f_expiry_date").toString();
        sid.qty = jo.value("f_qty").toString().toDouble();
        sid.price = jo.value("f_price").toString().toDouble();

        return sid;
    }

    static StoreUser *pointerFromJson(const QJsonObject &jo) { return new StoreUser(fromJson(jo)); }
};

template<>
struct JsonParser<StoreInputDocument>
{
    static StoreInputDocument fromJson(const QJsonObject &jo)
    {
        StoreInputDocument sid;
        sid.uuid = jo.value("f_id").toString();
        sid.user_id = jo.value("f_user_id").toString();
        sid.date = jo.value("f_doc_date").toString();
        sid.status = jo.value("f_status").toInt();
        sid.type = jo.value("f_doc_type").toInt();
        sid.store_in = jo.value("f_store_in").toInt();
        sid.store_in_name = jo.value("f_store_in_name").toString();
        sid.store_out = jo.value("f_store_out").toInt();
        sid.store_out_name = jo.value("f_store_out_name").toString();
        sid.sum = jo.value("f_sum").toString().toDouble();
        sid.version = jo.value("f_version").toInt();
        sid.partner = jo.value("f_partner").toInt();
        sid.parseData(jo);
        QJsonArray ji = jo.value("items").toArray();
        for (int i = 0; i < ji.size(); i++) {
            StoreUser su = JsonParser<StoreUser>::fromJson(ji.at(i).toObject());
            sid.items.append(su);
        }
        return sid;
    }

    static StoreInputDocument *pointerFromJson(const QJsonObject &jo) { return new StoreInputDocument(fromJson(jo)); }
};
