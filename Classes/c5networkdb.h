#ifndef C5NETWORKDB_H
#define C5NETWORKDB_H

#include <QObject>
#include <QJsonObject>

class C5NetworkDB : public QObject
{
    Q_OBJECT
public:
    explicit C5NetworkDB(const QString &sql, const QString &host, QObject *parent = nullptr);
    bool query();
    QString fLastError;
    QJsonObject fJsonOut;

private:
    QString fHost;
    QString fSql;

signals:

};

#endif // C5NETWORKDB_H
