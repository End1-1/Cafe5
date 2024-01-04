#ifndef STOREMANAGER_H
#define STOREMANAGER_H

#include <QString>
#include <QHash>
#include <QSettings>

class StoreManager
{
public:
    StoreManager();
    static void init(const QString &databaseName);
    static void release();
    static int queryQty(int store, const QStringList &sku, QJsonArray &out);
    static int queryQty(const QStringList &sku, QMap<int, QMap<QString, double> > &out);
    static int codeOfSku(const QString &sku);
    static QString nameOfSku(const QString &sku);

private:
    QString fDatabaseName;
    static StoreManager *fInstance;
    QHash<QString, int> fSkuCodeMap;
    QHash<int, QString> fCodeSkuMap;
    QHash<QString, QString> fSkuNameMap;
    QSettings s;
};

#endif // STOREMANAGER_H
