#pragma once

#include "c5jsonparser.h"
#include <QStringList>

struct GoodsGroupItem {
    int id;
    int parentId;
    int dept;
    int color;
    QString name;
    QString nameLower;
    QStringList words;
    QVector<GoodsGroupItem*> children;
};

template<>
struct JsonParser<GoodsGroupItem> {
    static GoodsGroupItem fromJson(const QJsonObject &jo)
    {
        return GoodsGroupItem{
            jo["f_id"].toInt(),
            jo["f_parent"].toInt(),
            jo["f_class"].toInt(),
            jo["f_color"].toInt(),
            jo["f_name"].toString()
        };
    }

    static GoodsGroupItem* pointerFromJson(const QJsonObject &jo)
    {
        return new GoodsGroupItem(fromJson(jo));
    }
};
