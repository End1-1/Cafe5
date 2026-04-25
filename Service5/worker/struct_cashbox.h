#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StructCashbox
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructCashbox>
{
    static constexpr const char *value = "search_cashbox";
};

template<>
struct JsonParser<StructCashbox>
{
    static StructCashbox fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StructCashbox>
{
    static QVariant id(const StructCashbox &s) { return s.id; }

    static QVariant name(const StructCashbox &s) { return s.name; }

    static constexpr ColumnDef<StructCashbox> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructCashbox &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
