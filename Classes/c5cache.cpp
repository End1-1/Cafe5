#include "c5cache.h"
#include "c5database.h"

QMap<QString, C5Cache*> C5Cache::fCacheList;
QMap<int, QString> C5Cache::fCacheQuery;
QMap<QString, int> C5Cache::fTableCache;

C5Cache::C5Cache(const QStringList &dbParams) :
    fDBParams(dbParams)
{
    if (fCacheQuery.count() == 0) {
        setCacheSimpleQuery(cache_users_groups, "s_user_group");
        setCacheSimpleQuery(cache_users_states, "s_user_state");
        setCacheSimpleQuery(cache_dish_part1, "d_part1");
        fCacheQuery[cache_dish_part2] = QString("select p2.f_id `%1`, p2.f_name as `%2`, p1.f_name `%3` "
                                        "from d_part2 p2 "
                                        "left join d_part1 p1 on p1.f_id=p2.f_part ")
                .arg(tr("Code"))
                .arg(tr("Name"))
                .arg(tr("Part"));
        fCacheQuery[cache_goods_unit] = QString("select f_id as `%1`, f_name as `%2` from c_units")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_goods_group] = QString("select f_id as `%1`, f_name as `%2` from c_groups")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_goods] = QString("select g.f_id as `%1`, gg.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4`, \
                                           g.f_scancode as `%5` \
                                           from c_goods g \
                                           left join c_groups gg on gg.f_id=g.f_group \
                                           left join c_units as u on u.f_id=g.f_unit")
                .arg(tr("Code"))
                .arg(tr("Group"))
                .arg(tr("Name"))
                .arg(tr("Unit"))
                .arg(tr("Scancode"));
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
        fCacheQuery[cache_users] = QString("select f_id as `%1`, concat(f_last, ' ', f_first) as `%2` from s_user where f_state=1")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_waiter_printers] = QString("select f_id as `%1`, f_name as `%2` from d_printers")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_credit_card] = QString("select f_id as `%1`, f_name as `%2` from o_credit_card")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_dish_remove_reason] = QString("select f_id as `%1`, f_name as `%2` from o_dish_remove_reason")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_discount_client] = QString("select c.f_id as `%1`, concat(f_lastname, ' ', f_firstname) as `%2`, "
                                                     "bc.f_govnumber as `%3`, f_firstname as `%4`, f_lastname as `%5`, f_info as `%6` "
                                                     "from b_clients c "
                                                     "left join b_car bc on bc.f_costumer=c.f_id ")
                .arg(tr("Code"))
                .arg(tr("Full name"))
                .arg(tr("Gov. number"))
                .arg(tr("First name"))
                .arg(tr("Last name"))
                .arg(tr("Additional info"));
        fCacheQuery[cache_s_db] = QString("select f_id as `%1`,  f_name as `%2`, f_description as `%3` from s_db")
                .arg(tr("Code"))
                .arg(tr("Name"))
                .arg(tr("Description"));
        setCacheSimpleQuery(cache_settings_names, "s_settings_names");
        setCacheSimpleQuery(cache_hall_list, "h_halls");
        setCacheSimpleQuery(cache_dish_comments, "d_dish_comment");
        setCacheSimpleQuery(cache_menu_names, "d_menu_names");
        setCacheSimpleQuery(cache_halls, "h_halls");
        setCacheSimpleQuery(cache_tables, "h_tables");
        setCacheSimpleQuery(cache_dish_package, "d_package");
        setCacheSimpleQuery(cache_salary_shift, "s_salary_shift");
        fCacheQuery[cache_dish_package] = QString("select f_id as `%1`, f_name as `%2`, f_price as `%3` from d_package ")
                .arg(tr("Code"))
                .arg(tr("Name"))
                .arg(tr("Price"));
        fCacheQuery[cache_dish_state] = QString("select f_id as `%1`, f_name as `%2` from o_body_state")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_order_state] = QString("select f_id as `%1`, f_name as `%2` from o_state")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_goods_waste] = "";
        fCacheQuery[cache_cash_names] = QString("select f_id as `%1`, f_name as `%2` from e_cash_names")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_store_reason] = QString("select f_id as `%1`, f_name as `%2` from a_reason")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_dish] = QString("select d.f_id as `%1`, d.f_name as `%2`, p2.f_name as `%3` "
                                          "from d_dish d "
                                          "inner join d_part2 p2 on p2.f_id=d.f_part "
                                          "order by 3, 2 ")
               .arg(tr("Code"))
               .arg(tr("Name"))
               .arg(tr("Type"));
        fCacheQuery[cache_discount_type] = QString("select f_id as `%1`, f_name as `%2` from b_card_types")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_header_payment] = QString("select f_id as `%1`, f_name as `%2` from a_header_payment")
                .arg(tr("Code"))
                .arg(tr("Name"));;
        fCacheQuery[cache_header_paid] = QString("select f_id as `%1`, f_name as `%2` from a_header_paid")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_goods_classes] = QString("select f_id as `%1`, f_name as `%2` from c_goods_classes")
                .arg(tr("Code"))
                .arg(tr("Name"));
        fCacheQuery[cache_dish_menu_state] = QString("select f_id as `%1`, f_name as `%2` from d_dish_state")
                .arg(tr("Code"))
                .arg(tr("Name"));
    }
    if (fTableCache.count() == 0) {
        fTableCache["c_partners"] = cache_goods_partners;
        fTableCache["c_units"] = cache_goods_unit;
        fTableCache["c_groups"] = cache_goods_group;
        fTableCache["c_goods"] = cache_goods;
        fTableCache["d_part1"] = cache_dish_part1;
        fTableCache["d_part2"] = cache_dish_part2;
        fTableCache["d_dish"] = cache_dish;
        fTableCache["c_storages"] = cache_goods_store;
        fTableCache["c_goods_waste"] = cache_goods_waste;
        fTableCache["s_user"] = cache_users;
        fTableCache["s_user_group"] = cache_users_groups;
        fTableCache["o_credit_card"] = cache_credit_card;
        fTableCache["o_dish_remove_reason"] = cache_dish_remove_reason;
        fTableCache["s_settings_names"] = cache_settings_names;
        fTableCache["h_halls"] = cache_hall_list;
        fTableCache["b_clients"] = cache_discount_client;
        fTableCache["b_cards_discount"] = cache_discount_cards;
        fTableCache["d_dish_comment"] = cache_dish_comments;
        fTableCache["s_db"] = cache_s_db;
        fTableCache["d_menu_names"] = cache_menu_names;
        fTableCache["h_halls"] = cache_halls;
        fTableCache["h_tables"] = cache_tables;
        fTableCache["o_body_state"] = cache_dish_state;
        fTableCache["o_state"] = cache_order_state;
        fTableCache["e_cash_names"] = cache_cash_names;
        fTableCache["a_reason"] = cache_store_reason;
        fTableCache["b_card_types"] = cache_discount_type;
        fTableCache["a_header_payment"] = cache_header_payment;
        fTableCache["a_header_paid"] = cache_header_paid;
        fTableCache["d_package"] = cache_dish_package;
        fTableCache["s_salary_shift"] = cache_salary_shift;
        fTableCache["c_goods_classes"] = cache_goods_classes;
    }
    fVersion = 0;
    C5Database db(dbParams);
    db.startTransaction();
    db.exec("select f_id from s_cache for update");
    QList<int> missingCache;
    for (QMap<QString, int>::const_iterator it = fTableCache.begin(); it != fTableCache.end(); it++) {
        bool found = false;
        for (int  i = 0; i < db.rowCount(); i++) {
            if (db.getInt(i, "f_id") == it.value()) {
                found = true;
                break;
            }
        }
        if (!found) {
            missingCache << it.value();
        }
    }
    foreach (int k, missingCache) {
        db[":f_id"] = k;
        db[":f_version"] = 0;
        db.insert("s_cache", false);
    }
    db.commit();
}

QString C5Cache::getString(int id)
{
    int row = find(id);
    if (row > -1) {
        return getString(row, 1);
    } else {
        return "FIND ERROR";
    }
}

void C5Cache::refresh()
{
    loadFromDatabase(fCacheQuery[fId]);
}

C5Cache *C5Cache::cache(const QStringList &dbParams, int cacheId)
{
    QString name = cacheName(dbParams, cacheId);
    C5Cache *cache = nullptr;
    if (!fCacheList.contains(name)) {
        cache = new C5Cache(dbParams);
        cache->fId = cacheId;
        fCacheList[name] = cache;
    } else {
        cache = fCacheList[name];
    }
    if (cache->fCacheData.count() == 0) {
        QString query = fCacheQuery[cacheId];
        cache->loadFromDatabase(query);
    } else {
        C5Database db(cache->fDBParams);
        db[":f_id"] = cache->fId;
        db.exec("select f_version from s_cache where f_id=:f_id");
        if (db.nextRow()) {
            if (cache->fVersion != db.getInt(0)) {
                QString query = fCacheQuery[cacheId];
                cache->loadFromDatabase(query);
            }
        }
    }
    return cache;
}

QString C5Cache::cacheName(const QStringList &dbParams, int cacheId)
{
    return dbParams[0] + dbParams[1] + dbParams[2] + dbParams[3] + "-" + QString::number(cacheId);
}

void C5Cache::resetCache(const QStringList &dbParams, const QString &table)
{
    int cacheId = fTableCache[table];
    C5Cache *c = C5Cache::cache(dbParams, cacheId);
    c->fCacheData.clear();
    c->fCacheIdRow.clear();
}

QString C5Cache::query(int cacheId)
{
    return fCacheQuery[cacheId];
}

void C5Cache::loadFromDatabase(const QString &query)
{
    fCacheIdRow.clear();
    C5Database db(fDBParams);
    db.exec(query, fCacheData);
    for (int i = 0; i < fCacheData.count(); i++) {
        fCacheIdRow[fCacheData[i][0].toInt()] = i;
    }
    db[":f_id"] = fId;
    db.exec("select f_version from s_cache where f_id=:f_id");
    if (db.nextRow()) {
        fVersion = db.getInt(0);
    }
}

void C5Cache::setCacheSimpleQuery(int cacheId, const QString &table)
{
    fCacheQuery[cacheId] = QString("select f_id as `%1`, f_name as `%2` from %3 ")
            .arg(tr("Code"))
            .arg(tr("Name"))
            .arg(table);
}
