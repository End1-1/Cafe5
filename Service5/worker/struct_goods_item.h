#pragma once
#include "c5jsonparser.h"
#include "c5structtraits.h"
#include <QStringList>

struct GoodsItem {
    int id;
    QString groupName;
    QString name;
    QString barcode;
    QString nameLower;
    QStringList words;
};

template<>
struct JsonParser<GoodsItem> {
    static GoodsItem fromJson(const QJsonObject &jo)
    {
        GoodsItem g;
        g.id        = jo["f_id"].toInt();
        g.groupName = jo["f_group_name"].toString();
        g.name      = jo["f_name"].toString();
        g.barcode   = jo["f_barcode"].toString();
        return g;
    }
};

template<>
struct StructTraits<GoodsItem> {
    static QVariant id(const GoodsItem &s)     { return s.id; }
    static QVariant groupName(const GoodsItem &s) {return s.groupName;}
    static QVariant name(const GoodsItem &s)   { return s.name; }
    static QVariant barcode(const GoodsItem &s) {return s.barcode;}

    static constexpr ColumnDef<GoodsItem> columns[] = {
        { "ID", &id },
        { "Group name", &groupName},
        { "Name", &name },
        { "Barcode", &barcode}
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
