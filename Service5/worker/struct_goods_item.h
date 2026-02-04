#pragma once
#include "c5jsonparser.h"
#include "c5structtraits.h"
#include <QStringList>

struct GoodsItem {
    int id = 0;
    QString groupName;
    QString name;
    QString barcode;
    QString unitName;
    double lastInputPrice = 0;
    double price1 = 0;
    double price1disc = 0;
    double price2 = 0;
    double price2disc = 0;
    QString nameLower;
    QStringList words;
};

template<> struct SelectorName<GoodsItem> {
    static constexpr const char* value = "search_goods_item";
};

template<>
struct JsonParser<GoodsItem> {
    static GoodsItem fromJson(const QJsonObject &jo)
    {
        GoodsItem g;
        g.id        = jo["f_id"].toInt();
        g.groupName = jo["f_group_name"].toString();
        g.name      = jo["f_name"].toString();
        g.unitName  = jo["f_unit_name"].toString();
        g.barcode   = jo["f_barcode"].toString();
        g.lastInputPrice = jo["f_lastinput"].toDouble();
        g.price1    = jo["f_price1"].toDouble();
        g.price1disc = jo["f_price1disc"].toDouble();
        g.price2    = jo["f_price2"].toDouble();
        g.price2disc = jo["f_price2disc"].toDouble();
        return g;
    }
};

template<>
struct StructTraits<GoodsItem> {
    static QVariant id(const GoodsItem &s)     { return s.id; }
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
