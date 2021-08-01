#ifndef STOREMANAGER_H
#define STOREMANAGER_H

#include <QString>
#include <QHash>

class StoreManager
{
public:
    StoreManager();
    static void init(const QString &databaseName);
    static int queryQty(int store, const QStringList &sku, QMap<QString, double> &out);
    static int queryQty(const QStringList &sku, QMap<int, QMap<QString, double> > &out);
    static int codeOfSku(const QString &sku);
    static QString nameOfSku(const QString &sku);

private:
    QString fDatabaseName;
    static StoreManager *fInstance;
    QHash<QString, int> fSkuCodeMap;
    QHash<int, QString> fCodeSkuMap;
    QHash<QString, QString> fSkuNameMap;
};

#endif // STOREMANAGER_H
