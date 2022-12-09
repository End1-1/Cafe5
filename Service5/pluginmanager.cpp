#include "pluginmanager.h"
#include <QDir>
#include <QApplication>
#include <QLibrary>
#include <QMutex>

PluginManager *PluginManager::fInstance = nullptr;
typedef bool (*startPlugin) (const QString &);
typedef QString (*pluginUuid)();
QMutex fMutex;

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
        pluginUuid pu = reinterpret_cast<pluginUuid>(l->resolve("pluginUuid"));
        pluginhandler ph = reinterpret_cast<pluginhandler>(l->resolve("handler"));
        removesocketfromplugin rs = reinterpret_cast<removesocketfromplugin>(l->resolve("removeSocket"));
        QString pus;
        if (pu) {
            pus = pu();
        }
        if (!pus.isEmpty() && ph) {
            fInstance->fPluginHandlers[pus] = ph;
        }
        if (rs) {
            fInstance->fPluginGarbage[pus] = rs;
        }
    }
}

int PluginManager::runHandler(const QString &pluginuuid, RawMessage *m, const QByteArray &in)
{
    QMutexLocker ml(&fMutex);
    if (fPluginHandlers.contains(pluginuuid)) {
        fPluginHandlers[pluginuuid](m, in);
        return 3;
    } else {
        return 0;
    }
}

void PluginManager::removeSocket(SslSocket *s)
{
    for (QHash<QString, removesocketfromplugin>::const_iterator it = fInstance->fPluginGarbage.constBegin();
         it != fInstance->fPluginGarbage.constEnd(); it++) {
        it.value()(s);
    }
}
