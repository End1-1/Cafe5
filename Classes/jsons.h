#ifndef JSONS_H
#define JSONS_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

#define __jsonstr(x) QString(QJsonDocument(x).toJson(QJsonDocument::Compact))
#define __strjson(x) QJsonDocument::fromJson(x.toUtf8()).object()
#define __bytejson(x) QJsonDocument::fromJson(x).object()

#endif // JSONS_H
