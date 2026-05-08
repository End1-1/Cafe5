#pragma once

#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct GoodsGroupItem {
    int id;
    int parentId;
    int dept;
    int color;
    int queue;
    QString name;
    QString nameLower;
    QStringList words;
    QVector<GoodsGroupItem*> children;
};

template<>
struct JsonParser<GoodsGroupItem> {
    static GoodsGroupItem fromJson(const QJsonObject &jo)
    {
        GoodsGroupItem g;
        g.id = jo["f_id"].toInt();
        g.parentId = jo["f_parent"].toInt();
        g.queue = jo.value("f_queue").toInt();
        g.dept = jo["f_class"].toInt();
        g.color = jo["f_color"].toInt();
        g.name = jo["f_name"].toString();
        return g;
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
    static QVariant queue(const GoodsGroupItem &s) { return s.queue; }

    static constexpr ColumnDef<GoodsGroupItem> columns[] = {{"ID", &id}, {"Name", &name}, {"Dept", &dept}, {"Color", &color}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const GoodsGroupItem &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
