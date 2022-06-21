#include "pluginmanager.h"
#include <QDir>
#include <QApplication>
#include <QLibrary>

PluginManager *PluginManager::fInstance = nullptr;
typedef bool (*startPlugin) (const QString &);

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{

}

void PluginManager::init(const QString &configFileName)
{
    fInstance = new PluginManager();
    QDir dir(qApp->applicationDirPath() + "/plugins");
    QStringList dll = dir.entryList(QStringList() << "*.dll", QDir::Files);
    for (auto &s: dll) {
        QLibrary *l = new QLibrary(qApp->applicationDirPath() + "/plugins/" + s);
        if (!l->load()) {
            l->deleteLater();
            continue;
        }
        startPlugin s = reinterpret_cast<startPlugin>(l->resolve("startPlugin"));
        s(configFileName);
    }
}
