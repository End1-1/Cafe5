#pragma once

#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StoreDocTypeItem
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StoreDocTypeItem>
{
    static constexpr const char *value = "search_store_doc_type";
};

template<>
struct JsonParser<StoreDocTypeItem>
{
    static StoreDocTypeItem fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StoreDocTypeItem>
{
    static QVariant id(const StoreDocTypeItem &s) { return s.id; }
    static QVariant name(const StoreDocTypeItem &s) { return s.name; }

    static constexpr ColumnDef<StoreDocTypeItem> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StoreDocTypeItem &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
