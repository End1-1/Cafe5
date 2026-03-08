#pragma once

#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StoreDocStatusItem
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StoreDocStatusItem>
{
    static constexpr const char *value = "search_store_doc_status";
};

template<>
struct JsonParser<StoreDocStatusItem>
{
    static StoreDocStatusItem fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StoreDocStatusItem>
{
    static QVariant id(const StoreDocStatusItem &s) { return s.id; }
    static QVariant name(const StoreDocStatusItem &s) { return s.name; }

    static constexpr ColumnDef<StoreDocStatusItem> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StoreDocStatusItem &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
