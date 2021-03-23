#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QJsonObject>

class JsonHandler
{
public:
    JsonHandler();
    QVariant &operator[](const QString &name);
    QString toString() const;
    int length();

private:
    QJsonObject jo;
    QMap<QString, QVariant> fValues;
};

#endif // JSONHANDLER_H
