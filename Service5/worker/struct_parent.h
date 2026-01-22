#pragma once

#include <QJsonObject>
#include <QJsonDocument>

struct ParentItem {
    QJsonObject data;
    void parseData(const QJsonObject &jo, const QString &key = "f_data")
    {
        if(jo.contains(key) && jo[key].isString()) {
            QJsonParseError err;
            const QByteArray raw = jo[key].toString().toUtf8();
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
