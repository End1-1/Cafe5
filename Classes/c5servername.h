#ifndef C5SERVERNAME_H
#define C5SERVERNAME_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

class C5ServerName : public QObject
{
    Q_OBJECT
public:
    explicit C5ServerName(const QString &server, const QString &route, QObject *parent = nullptr);
    bool getServers(const QString &name = "");
    static QJsonArray mServers;
    QJsonObject mParams;

private:
    QString mServer;
    QString mRoute;
    QString fLastTextMessage;

signals:
    void messageReceived();
};

#endif // C5SERVERNAME_H
