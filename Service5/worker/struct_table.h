#pragma once

#include "c5jsonparser.h"
#include <QStringList>
#include <QJsonDocument>

struct TableItem {
    int id = 0;
    int tableState = 0;
    int hall = 0;
    int staff = 0;
    int specialConfigId = 0;
    double amount = 0;
    QString staffName;
    QString name;
    QString comment;
    QString nameLower;
    QStringList words;
    QJsonObject data;
    QString dateOpen() const
    {
        return data["f_date_open"].toString();
    }
    QString timeOpen() const
    {
        return data["f_time_open"].toString().left(5);
    }
};

template<>
struct JsonParser<TableItem> {
    static TableItem fromJson(const QJsonObject &jo)
    {
        TableItem ti;
        ti.id = jo["f_id"].toInt();
        ti.tableState = jo["f_table_state"].toInt();
        ti.hall = jo["f_hall"].toInt();
        ti.specialConfigId = jo["f_special_config"].toInt();
        ti.staff = jo["f_current_staff"].toInt();
        ti.amount = jo["f_amount"].toDouble();
        ti.staffName = jo["f_current_staff_name"].toString();
        ti.name = jo["f_name"].toString();
        ti.comment = jo["f_comment"].toString();

        if(jo.contains("f_data") && jo["f_data"].isString()) {
            QJsonParseError err;
            const QByteArray raw = jo["f_data"].toString().toUtf8();
            QJsonDocument doc = QJsonDocument::fromJson(raw, &err);

            if(err.error == QJsonParseError::NoError && doc.isObject()) {
                ti.data = doc.object();
            } else {
                qWarning() << "f_data parse error:" << err.errorString()
                           << "raw:" << raw;
            }
        }

        return ti;
    }
};
