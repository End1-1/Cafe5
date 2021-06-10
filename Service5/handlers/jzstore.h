#ifndef JZSTORE_H
#define JZSTORE_H

#include "requesthandler.h"

class JZStore : public RequestHandler
{
public:
    JZStore();

protected:
    virtual bool handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;
    virtual bool validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap) override;

private:
    bool requestGoodsGroups(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    bool requestStore(const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
    static QMap<int, QMap<QString, QString> > fConnections;
    static QMap<int, QString> fStorages;
    static QMap<int, QMap<int, QString> > fAsStorageMap;
    void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username, const QString &password);
};

#endif // JZSTORE_H
