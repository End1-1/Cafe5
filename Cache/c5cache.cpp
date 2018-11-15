#include "c5cache.h"

QMap<QString, C5Cache*> C5Cache::fCacheList;
QMap<int, QString> C5Cache::fCacheQuery;

C5Cache::C5Cache(const QStringList &dbParams) :
    fDb(dbParams)
{
    if (fCacheQuery.count() == 0) {
        fCacheQuery[cache_users_groups] = "select f_id, f_name from s_user_group order by f_name";
        fCacheQuery[cache_users_states] = "select f_id, f_name from s_user_state";
        fCacheQuery[cache_dish_part1] = "select f_id, f_name from d_part1";
        fCacheQuery[cache_dish_part2] = "select f_id, f_name from d_part2";
        fCacheQuery[cache_goods_unit] = "select f_id, f_name from c_units";
        fCacheQuery[cache_goods_group] = "select f_id, f_name from c_groups";
        fCacheQuery[cache_goods] = QString("select g.f_id as `%1`, gg.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4` \
                                           from c_goods g \
                                           left join c_groups gg on gg.f_id=g.f_group \
                                           left join c_units as u on u.f_id=g.f_unit")
                .arg(tr("Code"))
                .arg(tr("Group"))
                .arg(tr("Name"))
                .arg(tr("Unit"));
        fCacheQuery[cache_goods_store] = QString("select f_id as `%1`, f_name as `%2` from c_storages")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_goods_partners] = QString("select f_id as `%1`, f_taxname as `%2`, f_contact as `%3`, \
                                                    f_info as `%4`, f_phone `%5`, f_email as `%6` from c_partners ")
                                                    .arg(tr("Code"))
                                                    .arg(tr("Name"))
                                                    .arg(tr("Contact"))
                                                    .arg(tr("Info"))
                                                    .arg(tr("Phone"))
                                                    .arg(tr("Email"));
        fCacheQuery[cache_store_inout] = QString("select f_id as `%1`, f_name as `%2` from a_type_sign")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_doc_state] = QString("select f_id as `%1`, f_name as `%2` from a_state")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_doc_type] = QString("select f_id as `%1`, f_name as `%2` from a_type")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_users] = QString("select f_id as `%1`, concat(f_last, ' ', f_first) as `%2` from s_user")
                .arg(tr("Code"))
                .arg(tr("Tax name"))
                .arg(tr("Contact"));
        fCacheQuery[cache_waiter_printers] = QString("select f_id as `%1`, f_name as `%2` from d_printers")
                .arg(tr("Code"))
                .arg(tr("Name"));
    }
}

void C5Cache::refresh()
{
    loadFromDatabase(fCacheQuery[fId]);
}

C5Cache *C5Cache::cache(const QStringList &dbParams, int cacheId)
{
    QString name = cacheName(dbParams, cacheId);
    C5Cache *cache = 0;
    if (!fCacheList.contains(name)) {
        cache = new C5Cache(dbParams);
        cache->fId = cacheId;
        QString query = fCacheQuery[cacheId];
        cache->loadFromDatabase(query);
        fCacheList[name] = cache;
    }

    return fCacheList[name];
}

QString C5Cache::cacheName(const QStringList &dbParams, int cacheId)
{
    return dbParams[0] + dbParams[1] + dbParams[2] + dbParams[3] + "-" + QString::number(cacheId);
}

QString C5Cache::query(int cacheId)
{
    return fCacheQuery[cacheId];
}

void C5Cache::loadFromDatabase(const QString &query)
{
    fCacheIdRow.clear();
    fDb.exec(query, fCacheData);
    for (int i = 0; i < fCacheData.count(); i++) {
        fCacheIdRow[fCacheData[i][0].toInt()] = i;
    }
}
