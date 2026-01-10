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
        return DishAItem{
            jo["f_dish"].toInt(),
            jo["f_menu"].toInt(),
            jo["f_price"].toDouble(),
            jo["f_group"].toInt(),
            jo["f_store"].toInt(),
            jo["f_color"].toInt(),
            jo["f_taxdept"].toInt(),
            jo["f_stoplist"].toInt(),
            jo["f_group_name"].toString(),
            jo["f_goods_name"].toString(),
            jo["f_print1"].toString(),
            jo["f_print2"].toString(),
            jo["f_adgt"].toString(),
            jo["f_recent"].toInt() == 1,
            jo["f_emark_required"].toInt() == 1,
            QJsonDocument::fromJson(jo["f_data"].toString().toUtf8()).object()
        };
    }

    static DishAItem* pointerFromJson(const QJsonObject &jo)
    {
        return new DishAItem(fromJson(jo));
    }
};
