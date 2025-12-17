#ifndef C5SERVERNAME_H
#define C5SERVERNAME_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

class C5ServerName : public QObject
{
    Q_OBJECT
public:
    explicit C5ServerName(const QString &server, QObject *parent = nullptr);
    ~C5ServerName();
    bool getServers();
    bool getConnection(const QString &connectionName);
    static QJsonArray mServers;
    QJsonObject mReply;
    QString mErrorString;

private:
    QString mServer;
    QString fLastTextMessage;
    QTimer mTimer;

signals:
    void messageReceived();
};

#endif // C5SERVERNAME_H
