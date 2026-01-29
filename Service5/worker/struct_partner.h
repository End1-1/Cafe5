#pragma once
#include "c5jsonparser.h"
#include "c5structtraits.h"
#include <QStringList>

struct PartnerItem {
    int id;
    QString tin;
    QString taxName;
    QString contactName;
    QString phone;
    QString nameLower;
    QStringList words;
};

template<> struct SelectorName<PartnerItem> {
    static constexpr const char* value = "search_partner_item";
};

template<>
struct JsonParser<PartnerItem> {
    static PartnerItem fromJson(const QJsonObject &jo)
    {
        PartnerItem g;
        g.id = jo["f_id"].toInt();
        g.tin = jo["f_taxcode"].toString();
        g.contactName = jo["f_name"].toString();
        g.taxName = jo["f_taxname"].toString();
        g.phone = jo["f_phone"].toString();
        return g;
    }
};

template<>
struct StructTraits<PartnerItem> {
    static QVariant id(const PartnerItem &s)     { return s.id; }
    static QVariant tin(const PartnerItem &s) {return s.tin;}
    static QVariant taxName(const PartnerItem &s)   { return s.taxName; }
    static QVariant contactName(const PartnerItem &s) {return s.contactName;}
    static QVariant phone(const PartnerItem &p) {return p.phone;}

    static constexpr ColumnDef<PartnerItem> columns[] = {
        { "ID", &id },
        { "TIN", &tin},
        { "Tax name", &taxName },
        { "Contact", &contactName},
        {"Phone", &phone}
    };

    static constexpr int columnCount()
    {
        return sizeof(columns) / sizeof(columns[0]);
    }

    static QVariant data(const PartnerItem &item, int col)
    {
        return columns[col].getter(item);
    }

    static QVariant header(int col)
    {
        return columns[col].header;
    }
};
