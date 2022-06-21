#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);

    static void init(const QString &configFileName);

private:
    static PluginManager *fInstance;

signals:

};

#endif // PLUGINMANAGER_H
