#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StructCurrency
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructCurrency>
{
    static constexpr const char *value = "search_currency";
};

template<>
struct JsonParser<StructCurrency>
{
    static StructCurrency fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StructCurrency>
{
    static QVariant id(const StructCurrency &s) { return s.id; }

    static QVariant name(const StructCurrency &s) { return s.name; }

    static constexpr ColumnDef<StructCurrency> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructCurrency &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
