#pragma once

#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct GoodsGroupItem {
    int id;
    int parentId;
    int dept;
    int color;
    QString name;
    QString nameLower;
    QStringList words;
    QVector<GoodsGroupItem*> children;
};

template<>
struct JsonParser<GoodsGroupItem> {
    static GoodsGroupItem fromJson(const QJsonObject &jo)
    {
        return GoodsGroupItem{
            jo["f_id"].toInt(),
            jo["f_parent"].toInt(),
            jo["f_class"].toInt(),
            jo["f_color"].toInt(),
            jo["f_name"].toString()
        };
    }

    static GoodsGroupItem* pointerFromJson(const QJsonObject &jo)
    {
        return new GoodsGroupItem(fromJson(jo));
    }
};

template<>
struct SelectorName<GoodsGroupItem>
{
    static constexpr const char *value = "search_goods_group";
};

template<>
struct StructTraits<GoodsGroupItem>
{
    static QVariant id(const GoodsGroupItem &s) { return s.id; }
    static QVariant name(const GoodsGroupItem &s) { return s.name; }
    static QVariant dept(const GoodsGroupItem &s) { return s.dept; };
    static QVariant color(const GoodsGroupItem &s) { return s.color; }

    static constexpr ColumnDef<GoodsGroupItem> columns[] = {{"ID", &id}, {"Name", &name}, {"Dept", &dept}, {"Color", &color}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const GoodsGroupItem &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
