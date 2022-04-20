#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class ServerConnection {
public:
    ServerConnection();
    static void getParams(QString &ip, int &port, QString &username, QString &password);
    static void setParams(const QString &ip, const QString &port, const QString &username, const QString &password);

private:
    static ServerConnection *fInstance;
    QString fIP;
    QString fPort;
    QString fUsername;
    QString fPassword;
};

#endif // CONFIG_H
