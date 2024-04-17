#ifndef C5SERVERNAME_H
#define C5SERVERNAME_H

#include <QObject>
#include <QJsonArray>

class C5ServerName : public QObject
{
    Q_OBJECT
public:
    explicit C5ServerName(const QString &server, QObject *parent = nullptr);

    void getServers();

    static QJsonArray mServers;

private:
    QString mServer;
signals:

};

#endif // C5SERVERNAME_H
