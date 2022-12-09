#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include "rawmessage.h"

typedef int (*pluginhandler) (RawMessage *rm, const QByteArray &in);
typedef int (*removesocketfromplugin) (SslSocket *s);

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);

    static void init(const QString &configFileName);

    static PluginManager *fInstance;

    int runHandler(const QString &pluginuuid, RawMessage *m, const QByteArray &in);

    void removeSocket(SslSocket *s);

private:

    QHash<QString, pluginhandler> fPluginHandlers;

    QHash<QString, removesocketfromplugin> fPluginGarbage;

signals:

};

#endif // PLUGINMANAGER_H
