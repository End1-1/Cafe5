#include "serverconfig.h"

#include "ndataprovider.h"
#include "ninterface.h"
#include "version.h"

#include <QJsonObject>
#include "selfboardsettings.h"

#include <QSettings>
#include <QUrl>

namespace {

QString appFileVersion()
{
    return QStringLiteral("%1.%2.%3.%4")
        .arg(VER_MAJOR)
        .arg(VER_MINOR)
        .arg(VER_PATCH)
        .arg(VER_BUILD);
}

QString normalizeHost(const QString &raw)
{
    QString host = raw.trimmed();
    if (host.startsWith(QStringLiteral("http://"), Qt::CaseInsensitive)) {
        host = host.mid(7);
    } else if (host.startsWith(QStringLiteral("https://"), Qt::CaseInsensitive)) {
        host = host.mid(8);
    }
    while (host.endsWith('/')) {
        host.chop(1);
    }
    return host;
}

} // namespace

ServerConfig &ServerConfig::instance()
{
    static ServerConfig cfg;
    return cfg;
}

void ServerConfig::loadFromSettings()
{
    QSettings s = SelfBoardSettings::store();
    auto &cfg = instance();
    cfg.m_host = s.value(QStringLiteral("serverHost")).toString();
    cfg.m_https = s.value(QStringLiteral("serverProtocol"), QStringLiteral("https")).toString()
                  != QStringLiteral("http");
    cfg.m_username = s.value(QStringLiteral("serverUser")).toString();
    cfg.m_password = s.value(QStringLiteral("serverPassword")).toString();
    cfg.m_sessionKey = s.value(QStringLiteral("sessionKey")).toString();
}

void ServerConfig::saveToSettings()
{
    const auto &cfg = instance();
    QSettings s = SelfBoardSettings::store();
    s.setValue(QStringLiteral("serverHost"), cfg.m_host);
    s.setValue(QStringLiteral("serverProtocol"), cfg.m_https ? QStringLiteral("https") : QStringLiteral("http"));
    s.setValue(QStringLiteral("serverUser"), cfg.m_username);
    s.setValue(QStringLiteral("serverPassword"), cfg.m_password);
    s.setValue(QStringLiteral("sessionKey"), cfg.m_sessionKey);
    SelfBoardSettings::flush();
}

bool ServerConfig::isConfigured()
{
    const auto &cfg = instance();
    return !cfg.m_host.trimmed().isEmpty()
        && !cfg.m_username.trimmed().isEmpty()
        && !cfg.m_password.isEmpty();
}

bool ServerConfig::hasSession()
{
    return !instance().m_sessionKey.trimmed().isEmpty();
}

QString ServerConfig::host() { return instance().m_host; }
bool ServerConfig::useHttps() { return instance().m_https; }
QString ServerConfig::username() { return instance().m_username; }
QString ServerConfig::password() { return instance().m_password; }
QString ServerConfig::sessionKey() { return instance().m_sessionKey; }

void ServerConfig::setHost(const QString &host) { instance().m_host = normalizeHost(host); }
void ServerConfig::setUseHttps(bool https) { instance().m_https = https; }
void ServerConfig::setUsername(const QString &user) { instance().m_username = user.trimmed(); }
void ServerConfig::setPassword(const QString &password) { instance().m_password = password; }
void ServerConfig::setSessionKey(const QString &key) { instance().m_sessionKey = key.trimmed(); }

void ServerConfig::applyToNetwork()
{
    const auto &cfg = instance();
    NDataProvider::mAppName = QStringLiteral("selfboard");
    NDataProvider::mFileVersion = appFileVersion();
    NDataProvider::mProtocol = cfg.m_https ? QStringLiteral("https") : QStringLiteral("http");
    NDataProvider::mHost = cfg.m_host;
    NDataProvider::sessionKey = cfg.m_sessionKey;
}

void ServerConfig::login(QObject *context, std::function<void(bool, const QString &)> finished)
{
    auto &cfg = instance();
    cfg.m_host = normalizeHost(cfg.m_host);
    if (cfg.m_host.isEmpty()) {
        finished(false, QObject::tr("Server address is required"));
        return;
    }
    if (cfg.m_username.isEmpty() || cfg.m_password.isEmpty()) {
        finished(false, QObject::tr("Login and password are required"));
        return;
    }

    applyToNetwork();

    NInterface::query(
        QStringLiteral("/engine/v2/worker/user-login/login"),
        QString(),
        context,
        QJsonObject{
            {QStringLiteral("username"), cfg.m_username},
            {QStringLiteral("password"), cfg.m_password},
            {QStringLiteral("nootp"), true},
        },
        [finished](const QJsonObject &jdoc) {
            if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
                const QString msg = jdoc.value(QStringLiteral("message")).toString();
                finished(false, msg.isEmpty() ? QObject::tr("Login failed") : msg);
                return;
            }

            QString token = jdoc.value(QStringLiteral("token")).toString();
            if (token.isEmpty()) {
                token = jdoc.value(QStringLiteral("sessionkey")).toString();
            }
            if (token.isEmpty()) {
                finished(false, QObject::tr("Login succeeded but token is empty"));
                return;
            }

            instance().m_sessionKey = token;
            saveToSettings();
            applyToNetwork();
            finished(true, QString());
        },
        [finished](const QJsonObject &jerr) -> bool {
            const QString msg = jerr.value(QStringLiteral("errorMessage")).toString();
            finished(false, msg.isEmpty() ? QObject::tr("Login failed") : msg);
            return true;
        },
        true,
        60000,
        true);
}
