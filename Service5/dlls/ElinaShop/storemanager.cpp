#include "storemanager.h"
#include "database.h"
#include "logwriter.h"
#include <QElapsedTimer>
#include <QApplication>
#include <QDebug>

StoreManager *StoreManager::fInstance = nullptr;

StoreManager::StoreManager() :
    s(qApp->applicationDirPath() + "/handlers/shop.ini", QSettings::IniFormat)
{
    qDebug() << s.allKeys();
}

void StoreManager::init(const QString &databaseName)
{
    QElapsedTimer e;
    e.start();
    if (fInstance == nullptr) {
        fInstance = new StoreManager();
    }
    fInstance->fDatabaseName = databaseName;
    Database db;
    if (db.open(databaseName) == false) {
        return;
    }
    db.exec("select f_id, f_name, f_scancode from c_goods");
    while (db.next()) {
        fInstance->fSkuCodeMap.insert(db.string("f_scancode"), db.integer("f_id"));
        fInstance->fCodeSkuMap.insert(db.integer("f_id"), db.string("f_scancode"));
        fInstance->fSkuNameMap.insert(db.string("f_scancode"), db.string("f_name"));
    }
}

void StoreManager::release()
{
    if (fInstance) {
        delete fInstance;
    }
}

int StoreManager::queryQty(int store, const QStringList &sku, QJsonArray &out)
{
    Database db;
    if (db.open(fInstance->fDatabaseName) == false) {
        return -1;
    }

    QStringList codes;
    for (const QString &s: sku) {
        if (fInstance->fSkuCodeMap.contains(s)) {
            codes.append(QString::number(fInstance->fSkuCodeMap.value(s)));
        }
    }
    if (codes.empty()) {
        return -1;
    }
    QString sql = QString("select  s.f_goods, gp.f_price1, sum(s.f_qty*s.f_type) as f_qty "
                    "from a_store s "
                    "inner join a_header h on h.f_id=s.f_document "
                    "left join c_goods_price gp on gp.f_goods=s.f_goods and gp.f_currency = 1 "
                    "where h.f_date<=current_date() %store "
                    "and s.f_goods in (%1) "
                    "group by 1 "
                    "having sum(s.f_qty*s.f_type)>0 ")
            .arg(codes.join(","));
    if (store == 0) {
        QString storelist = fInstance->s.value("shop/store").toString() + "," + fInstance->s.value("shop/storeorder").toString();
        sql.replace("%store", QString("and s.f_store in (%1)").arg(storelist));
    } else {
        sql.replace("%store", QString("and s.f_store=%1").arg(store));
    }
    db.exec(sql);
    LogWriter::write(LogWriterLevel::verbose, "", sql);
    while (db.next()) {
        out.insert(fInstance->fCodeSkuMap.value(db.integer("f_goods")), db.doubleValue("f_qty"));
    }
    sql = "select f_goods, sum(f_qty) as f_qty from a_store_reserve where f_state=1 and f_enddate>=current_date %store group by 1";
    if (store == 0) {
        QString storelist = fInstance->s.value("shop/store").toString() + "," + fInstance->s.value("shop/storeorder").toString();
        sql.replace("%store", QString("and f_store in (%1)").arg(storelist));
    } else {
        sql.replace("%store", QString("and f_store=%1").arg(store));
    }
    db.exec(sql);
    while (db.next()) {
        if (out.contains(fInstance->fCodeSkuMap.value(db.integer("f_goods")))) {
            out[fInstance->fCodeSkuMap.value(db.integer("f_goods"))] = out[fInstance->fCodeSkuMap.value(db.integer("f_goods"))] - db.doubleValue("f_qty");
        }
    }
    return out.count();
}

int StoreManager::queryQty(const QStringList &sku, QMap<int, QMap<QString, double> > &out)
{
    Database db;
    if (db.open(fInstance->fDatabaseName) == false) {
        return -1;
    }

    QStringList codes;
    for (const QString &s: sku) {
        if (fInstance->fSkuCodeMap.contains(s)) {
            codes.append(QString::number(fInstance->fSkuCodeMap.value(s)));
        }
    }
    if (codes.empty()) {
        return -1;
    }
    QString storelist = fInstance->s.value("shop/store").toString() + "," + fInstance->s.value("shop/storeorder").toString();
    QString sql = QString("select s.f_store, s.f_goods, sum(s.f_qty*s.f_type) as f_qty "
                    "from a_store s "
                    "inner join a_header h on h.f_id=s.f_document "
                    "where h.f_date<=current_date()  and h.f_state=1  "
                    "and s.f_goods in (%1) and s.f_store in(%2) "
                    "group by 1, 2 "
                    "having sum(s.f_qty*s.f_type)>0 ")
            .arg(codes.join(","), storelist);

    db.exec(sql);
    LogWriter::write(LogWriterLevel::verbose, "", sql);
    while (db.next()) {
        out[db.integer("f_store")].insert(fInstance->fCodeSkuMap.value(db.integer("f_goods")), db.doubleValue("f_qty"));
    }
    db.exec("select f_store, f_goods, sum(f_qty) as f_qty from a_store_reserve where f_state=1 and f_enddate>=current_date group by 1, 2");
    while (db.next()) {
        if (out.contains(db.integer("f_store"))) {
            if (out[db.integer("f_store")].contains(fInstance->fCodeSkuMap.value(db.integer("f_goods")))) {
                out[db.integer("f_store")][fInstance->fCodeSkuMap.value(db.integer("f_goods"))] = out[db.integer("f_store")][fInstance->fCodeSkuMap.value(db.integer("f_goods"))] - db.doubleValue("f_qty");
            }
        }
    }
    return out.count();
}

int StoreManager::codeOfSku(const QString &sku)
{
    return fInstance->fSkuCodeMap[sku];
}

QString StoreManager::nameOfSku(const QString &sku)
{
    return fInstance->fSkuNameMap[sku];
}
