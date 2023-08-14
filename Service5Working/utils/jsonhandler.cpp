#include "jsonhandler.h"
#include <QJsonDocument>

JsonHandler::JsonHandler()
{

}

QVariant &JsonHandler::operator[](const QString &name)
{
    return fValues[name];
}

QString JsonHandler::toString() const
{
    QJsonDocument jdoc = QJsonDocument::fromVariant(fValues);
    return jdoc.toJson(QJsonDocument::Compact);
}

int JsonHandler::length()
{
    return toString().toUtf8().length();
}
