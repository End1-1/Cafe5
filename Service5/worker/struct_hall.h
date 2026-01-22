#pragma once

#include "c5jsonparser.h"
#include <QStringList>
#include <QJsonDocument>

struct HallItem {
    int id = 0;
    int configId = 0;
    QString name;
    QString nameLower;
    QStringList words;
    QJsonObject data;
    double serviceFactor() const
    {
        return data["service_factor"].toDouble();
    }
    int defaultHall() const
    {
        return data["default_hall"].toInt();
    }
    int defaultMenu() const
    {
        return data["default_menu"].toInt();
    }
};

template<>
struct JsonParser<HallItem> {
    static HallItem fromJson(const QJsonObject &jo)
    {
        HallItem hi;
        hi.id = jo["f_id"].toInt();
        hi.configId = jo["f_settings"].toInt();
        hi.name = jo["f_name"].toString();

        if(jo.contains("f_data") && jo["f_data"].isString()) {
            QJsonParseError err;
            const QByteArray raw = jo["f_data"].toString().toUtf8();
            QJsonDocument doc = QJsonDocument::fromJson(raw, &err);

            if(err.error == QJsonParseError::NoError && doc.isObject()) {
                hi.data = doc.object();
            } else {
                qWarning() << "f_data parse error:" << err.errorString()
                           << "raw:" << raw;
            }
        }

        return hi;
    }
};
