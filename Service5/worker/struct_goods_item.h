#pragma once
#include "c5jsonparser.h"
#include "c5structtraits.h"
#include <QStringList>

struct GoodsItem {
    int id = 0;
    int groupId = 0;
    QString groupName;
    QString name;
    QString barcode;
    QString unitName;
    QString adgt;
    double lastInputPrice = 0;
    double price1 = 0;
    double price1disc = 0;
    double price2 = 0;
    double price2disc = 0;
    double qty = 0;
    QString nameLower;
    QStringList words;

    QJsonObject toJson() const
    {
        return QJsonObject{{"f_id", id},
                           {"f_group_id", groupId},
                           {"f_group_name", groupName},
                           {"f_name", name},
                           {"f_barcode", barcode},
                           {"f_unit_name", unitName},
                           {"f_price1", price1},
                           {"f_price1disc", price1disc},
                           {"f_price2", price2},
                           {"f_price2disc", price2disc},
                           {"f_qty", qty},
                           {"f_adgt", adgt}};
    }
};

template<> struct SelectorName<GoodsItem> {
    static constexpr const char* value = "search_goods_item";
};

template<>
struct JsonParser<GoodsItem> {
    static GoodsItem fromJson(const QJsonObject &jo)
    {
        GoodsItem g;
        g.id = jo.value("f_id").toInt();
        g.groupId = jo.value("f_group_id").toInt();
        g.groupName = jo.value("f_group_name").toString();
        g.name = jo.value("f_name").toString();
        g.unitName = jo.value("f_unit_name").toString();
        g.barcode = jo.value("f_barcode").toString();
        g.lastInputPrice = jo.value("f_lastinput").toDouble();
        g.price1 = jo.value("f_price1").toDouble();
        g.price1disc = jo.value("f_price1disc").toDouble();
        g.price2 = jo.value("f_price2").toDouble();
        g.price2disc = jo.value("f_price2disc").toDouble();
        g.qty = jo.value("f_qty").toDouble();
        g.adgt = jo.value("f_adgt").toString();
        return g;
    }
};

template<>
struct StructTraits<GoodsItem> {
    static QVariant id(const GoodsItem &s)     { return s.id; }
    static QVariant groupId(const GoodsItem &s) { return s.groupId; };
    static QVariant groupName(const GoodsItem &s) {return s.groupName;}
    static QVariant name(const GoodsItem &s)   { return s.name; }
    static QVariant barcode(const GoodsItem &s) {return s.barcode;}
    static QVariant unitName(const GoodsItem &s) {return s.unitName;}

    static constexpr ColumnDef<GoodsItem> columns[] = {
        {"ID", &id },
        {"Group name", &groupName},
        {"Name", &name},
        {"Unit", &unitName},
        {"Barcode", &barcode}
    };

    static constexpr int columnCount()
    {
        return sizeof(columns) / sizeof(columns[0]);
    }

    static QVariant data(const GoodsItem &item, int col)
    {
        return columns[col].getter(item);
    }

    static QVariant header(int col)
    {
        return columns[col].header;
    }
};
