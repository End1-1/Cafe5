#pragma once
#include "c5jsonparser.h"
#include <QStringList>
#include <QJsonDocument>

struct DishAItem {
    int id;
    int menuId;
    double price;
    int group;
    int store;
    int color;
    int fiscalDept;
    int stoplist;
    QString groupName;
    QString name;
    QString print1;
    QString print2;
    QString adgt;
    bool favorite;
    bool emarkRequired;
    QJsonObject data;
    QStringList barcodes;
    QString nameLower;
    QStringList words;
    bool countService()
    {
        return data["f_count_service"].toBool();
    }
    bool countDiscount()
    {
        return data["f_count_discount"].toBool();
    }
};

template<>
struct JsonParser<DishAItem> {
    static DishAItem fromJson(const QJsonObject &jo)
    {
        DishAItem di;
        di.id = jo.value("f_dish").toInt();
        di.menuId = jo.value("f_menu").toInt();
        di.price = jo.value("f_price").toDouble();
        di.group = jo.value("f_group").toInt();
        di.store = jo.value("f_store").toInt();
        di.color = jo.value("f_color").toInt();
        di.fiscalDept = jo.value("f_taxdept").toInt();
        di.stoplist = jo.value("f_stoplist").toInt();
        di.groupName = jo.value("f_group_name").toString();
        di.name = jo.value("f_goods_name").toString();
        di.print1 = jo.value("f_print1").toString();
        di.print2 = jo.value("f_print2").toString();
        di.adgt = jo.value("f_adgt").toString();
        di.favorite = jo.value("f_recent").toInt() == 1;
        di.emarkRequired = jo.value("f_emark_required").toInt() == 1;
        di.data = QJsonDocument::fromJson(jo.value("f_data").toString().toUtf8()).object();
        di.barcodes = jo.value("f_scancode").toString().split(",", Qt::SkipEmptyParts);
        di.nameLower = di.name.toLower();

        return di;
    }

    static DishAItem* pointerFromJson(const QJsonObject &jo)
    {
        return new DishAItem(fromJson(jo));
    }
};
