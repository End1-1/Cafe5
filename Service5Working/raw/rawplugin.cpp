#include "rawplugin.h"
#include "pluginmanager.h"

RawPlugin::RawPlugin(SslSocket *s) :
    Raw(s)
{

}

int RawPlugin::run(const QByteArray &d)
{
    QString pluginUuid;
    readString(pluginUuid, d);
    if (pluginUuid.isEmpty()) {
        putUByte(0);
        putString("Unknown plugin uuid");
        return 0;
    }

    return PluginManager::fInstance->runHandler(pluginUuid, this, d);
}
