#pragma once

#include "c5jsonparser.h"
#include "c5structtraits.h"
#include "struct_parent.h"
#include <QStringList>

struct StructEmployeeGroup : ParentItem
{
    int id = 0;
    QString name;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructEmployeeGroup>
{
    static constexpr const char *value = "search_employee_group";
};

template<>
struct JsonParser<StructEmployeeGroup>
{
    static StructEmployeeGroup fromJson(const QJsonObject &jo)
    {
        StructEmployeeGroup s;
        s.id = jo["f_id"].toInt();
        s.name = jo["f_name"].toString();
        return s;
    }
};

template<>
struct StructTraits<StructEmployeeGroup>
{
    static QVariant id(const StructEmployeeGroup &s) { return s.id; }
    static QVariant name(const StructEmployeeGroup &s) { return s.name; }

    static constexpr ColumnDef<StructEmployeeGroup> columns[] = {
        {"ID", &id},
        {"Name", &name}
    };

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructEmployeeGroup &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
