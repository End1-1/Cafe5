#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QObject>
#include <QString>
#include <functional>

class ServerConfig
{
public:
    static void loadFromSettings();
    static void saveToSettings();

    static bool isConfigured();
    static bool hasSession();

    static QString host();
    static bool useHttps();
    static QString username();
    static QString password();
    static QString sessionKey();

    static void setHost(const QString &host);
    static void setUseHttps(bool https);
    static void setUsername(const QString &user);
    static void setPassword(const QString &password);
    static void setSessionKey(const QString &key);

    static void applyToNetwork();

    static void login(QObject *context,
                      std::function<void(bool ok, const QString &error)> finished);

private:
    static ServerConfig &instance();

    QString m_host;
    bool m_https = true;
    QString m_username;
    QString m_password;
    QString m_sessionKey;
};

#endif // SERVERCONFIG_H
