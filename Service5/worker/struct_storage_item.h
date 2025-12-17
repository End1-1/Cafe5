#pragma once
#include "c5jsonparser.h"
#include "c5structtraits.h"
#include <QStringList>

struct StorageItem {
    int id;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct JsonParser<StorageItem> {
    static StorageItem fromJson(const QJsonObject &jo)
    {
        return {
            jo["f_id"].toInt(),
            jo["f_name"].toString()
        };
    }
};

template<>
struct StructTraits<StorageItem> {
    static QVariant id(const StorageItem &s)     { return s.id; }
    static QVariant name(const StorageItem &s)   { return s.name; }

    static constexpr ColumnDef<StorageItem> columns[] = {
        { "ID", &id },
        { "Name", &name }
    };

    static constexpr int columnCount()
    {
        return sizeof(columns) / sizeof(columns[0]);
    }

    static QVariant data(const StorageItem &item, int col)
    {
        return columns[col].getter(item);
    }

    static QVariant header(int col)
    {
        return columns[col].header;
    }
};
