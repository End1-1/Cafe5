#pragma once
#include <QJsonObject>
#include <QJsonArray>

template<typename T>
struct JsonParser;

template<typename T>
QVector<T> parseJsonArray(const QJsonArray &arr)
{
    QVector<T> result;
    result.reserve(arr.size());

    for(const QJsonValue &v : arr) {
        result.append(JsonParser<T>::fromJson(v.toObject()));
    }

    return result;
}
