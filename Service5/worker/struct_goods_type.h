#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StructGoodsType
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructGoodsType>
{
    static constexpr const char *value = "search_goods_type";
};

template<>
struct JsonParser<StructGoodsType>
{
    static StructGoodsType fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StructGoodsType>
{
    static QVariant id(const StructGoodsType &s) { return s.id; }
    static QVariant name(const StructGoodsType &s) { return s.name; }

    static constexpr ColumnDef<StructGoodsType> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructGoodsType &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
