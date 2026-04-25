#ifndef HTTPLITE_H
#define HTTPLITE_H

#include <QObject>

class HttpLite : public QObject
{
    Q_OBJECT
public:
    explicit HttpLite(QObject *parent = nullptr);

    void post(const QString &url, const QJsonObject &obj);

signals:
};

#endif // HTTPLITE_H
