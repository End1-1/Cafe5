#include "selfboardsettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

namespace {

bool g_pathConfigured = false;

QString userSettingsPath()
{
    // %LOCALAPPDATA%\Jazzve\SelfBoard.ini
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
        + QStringLiteral("/Jazzve");
    QDir().mkpath(dir);
    return dir + QStringLiteral("/SelfBoard.ini");
}

QString bundledSettingsPath()
{
    // Installed next to SelfBoard.exe by the installer (demo defaults).
    return QCoreApplication::applicationDirPath() + QStringLiteral("/SelfBoard.ini");
}

void seedUserSettingsIfNeeded()
{
    const QString userIni = userSettingsPath();
    if (QFile::exists(userIni)) {
        QSettings existing(userIni, QSettings::IniFormat);
        if (!existing.value(QStringLiteral("serverHost")).toString().trimmed().isEmpty()) {
            return;
        }
    }

    const QString bundled = bundledSettingsPath();
    if (!QFile::exists(bundled)) {
        return;
    }

    if (QFile::exists(userIni)) {
        QFile::remove(userIni);
    }
    QFile::copy(bundled, userIni);
}

} // namespace

void SelfBoardSettings::configureStorage()
{
    if (g_pathConfigured) {
        return;
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);
    seedUserSettingsIfNeeded();
    g_pathConfigured = true;
}

QSettings SelfBoardSettings::store()
{
    configureStorage();
    return QSettings(userSettingsPath(), QSettings::IniFormat);
}

void SelfBoardSettings::flush()
{
    store().sync();
}
