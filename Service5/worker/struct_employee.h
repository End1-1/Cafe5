#pragma once

#include "c5jsonparser.h"
#include "c5structtraits.h"
#include "struct_parent.h"
#include <QStringList>

struct StructEmployee : ParentItem
{
    int id = 0;
    QString groupName;
    QString firstName;
    QString lastName;
    QString login;
    QString phone;
    QString nameLower;
    QStringList words;
};

template<>
struct SelectorName<StructEmployee>
{
    static constexpr const char *value = "search_employee";
};

template<>
struct JsonParser<StructEmployee>
{
    static StructEmployee fromJson(const QJsonObject &jo)
    {
        StructEmployee s;
        s.id = jo["f_id"].toInt();
        s.groupName = jo["f_group_name"].toString();
        s.firstName = jo["f_first"].toString();
        s.lastName = jo["f_last"].toString();
        s.login = jo["f_login"].toString();
        s.phone = jo["f_phone"].toString();
        return s;
    }
};

template<>
struct StructTraits<StructEmployee>
{
    static QVariant id(const StructEmployee &s) { return s.id; }
    static QVariant groupName(const StructEmployee &s) { return s.groupName; }
    static QVariant firstName(const StructEmployee &s) { return s.firstName; }
    static QVariant lastName(const StructEmployee &s) { return s.lastName; }
    static QVariant login(const StructEmployee &s) { return s.login; }
    static QVariant phone(const StructEmployee &s) { return s.phone; }

    static constexpr ColumnDef<StructEmployee> columns[] = {
        {"ID", &id},
        {"Group", &groupName},
        {"First", &firstName},
        {"Last", &lastName},
        {"Login", &login},
        {"Phone", &phone}
    };

    static constexpr int columnCount() { return sizeof(columns) / sizeof(columns[0]); }

    static QVariant data(const StructEmployee &item, int col) { return columns[col].getter(item); }

    static QVariant header(int col) { return columns[col].header; }
};
