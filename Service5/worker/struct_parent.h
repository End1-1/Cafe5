#pragma once

#include <QJsonObject>
#include <QJsonDocument>

struct ParentItem {
    QJsonObject data;
    void parseData(const QJsonObject &jo, const QString &key = "f_data")
    {
        if(!jo.contains(key)) {
            return;
        }

        const QJsonValue value = jo[key];
        if(value.isObject()) {
            data = value.toObject();
            return;
        }

        if(value.isString()) {
            QJsonParseError err;
            const QByteArray raw = value.toString().toUtf8();
            QJsonDocument doc = QJsonDocument::fromJson(raw, &err);

            if(err.error == QJsonParseError::NoError && doc.isObject()) {
                data = doc.object();
            } else {
                qWarning() << key << "parse error:" << err.errorString()
                           << "raw:" << raw;
            }
        }
    }
    virtual void validate() {}
};
