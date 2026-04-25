#pragma once
#include <QStringList>
#include "c5jsonparser.h"
#include "c5structtraits.h"

struct StructPaymentType
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructPaymentType>
{
    static constexpr const char *value = "search_payment_type";
};

template<>
struct JsonParser<StructPaymentType>
{
    static StructPaymentType fromJson(const QJsonObject &jo) { return {jo["f_id"].toInt(), jo["f_name"].toString()}; }
};

template<>
struct StructTraits<StructPaymentType>
{
    static QVariant id(const StructPaymentType &s) { return s.id; }

    static QVariant name(const StructPaymentType &s) { return s.name; }

    static constexpr ColumnDef<StructPaymentType> columns[] = {{"ID", &id}, {"Name", &name}};

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructPaymentType &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
