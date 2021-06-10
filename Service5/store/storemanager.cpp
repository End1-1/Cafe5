#include "storemanager.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include "store.h"
#include "debug.h"

StoreManager *StoreManager::fInstance = nullptr;
QMap<int, Store*> StoreManager::fStores;

StoreManager::StoreManager()
{

}

void StoreManager::init(const QString &databaseName)
{
    QElapsedTimer e;
    e.start();
    fInstance = new StoreManager();
    fInstance->fDatabaseName = databaseName;
    Database db;
    JsonHandler jh;
    if (!DatabaseConnectionManager::openDatabase(databaseName, db, jh)) {
        return;
    }
    db.exec("select d.f_date, s.f_store, s.f_base, s.f_goods, s.f_price, g.f_scancode, "
            "sum(s.f_qty*s.f_type) as f_qty,  sum(s.f_total*s.f_type) as f_total "
            "from a_store s  "
            "inner join a_header d on d.f_id=s.f_document "
            "left join c_goods g on g.f_id=s.f_goods "
            "where d.f_state=1 "
            "group by 1, 2, 3, 4, 5 "
            "having sum(s.f_qty*s.f_type) > 0.00001 ");
    while (db.next()) {
        Store *s = getStore(db.integerValue("f_store"));
        s->addRecord(db.dateValue("f_date"), db.stringValue("f_base"), db.integerValue("f_store"), db.stringValue("f_scancode"), db.integerValue("f_goods"), db.doubleValue("f_price"), db.doubleValue("f_qty"));
    }
    __debug_log(QString("Store initialization complete in %1 ms").arg(e.elapsed()));
}

Store *StoreManager::getStore(int id)
{
    if (fStores.contains(id)) {
        return fStores[id];
    }
    auto *s = new Store(id);
    fStores.insert(id, s);
    return s;
}

int StoreManager::queryQty(int store, const QStringList &sku, QList<StoreRecord> &sr)
{
    if (store == 0) {

    }
    for (Store *s: fStores) {
        if (store != 0) {
            if (s->fStore != store) {
                continue;
            }
        }
        for (const QString &g: sku) {
            QList<StoreRecord> records = s->getRecords(g);
            sr.append(records);
        }
    }
    return sr.count();
}
