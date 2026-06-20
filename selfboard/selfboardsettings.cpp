#include "selfboardsettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

namespace {

bool g_pathConfigured = false;

} // namespace

void SelfBoardSettings::configureStorage()
{
    if (g_pathConfigured) {
        return;
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, dir);
    g_pathConfigured = true;
}

QSettings SelfBoardSettings::store()
{
    configureStorage();
    return QSettings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
}

void SelfBoardSettings::flush()
{
    store().sync();
}
