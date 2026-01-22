#include "c5cache.h"
#include "c5database.h"

QMap<QString, C5Cache*> C5Cache::fCacheList;
QMap<int, QString> C5Cache::fCacheQuery;
QMap<QString, int> C5Cache::fTableCache;
QMap<int, QHash<QString, int> > C5Cache::fCacheColumns;

C5Cache::C5Cache()
{
    if(fCacheQuery.count() == 0) {
        setCacheSimpleQuery(cache_users_groups, "s_user_group");
        setCacheSimpleQuery(cache_users_states, "s_user_state");
        setCacheSimpleQuery(cache_dish_part1, "d_part1");
        fCacheQuery[cache_dish_part2] = QString("select p2.f_id `%1`, p2.f_name as `%2`, p1.f_name `%3` "
                                                "from d_part2 p2 "
                                                "left join d_part1 p1 on p1.f_id=p2.f_part ")
                                        .arg(tr("Code"), tr("Name"), tr("Part"));
        fCacheQuery[cache_goods_unit] = QString("select f_id as `%1`, f_name as `%2`, f_fullname as `%3` from c_units")
                                        .arg(tr("Code"), tr("Name"), tr("Full caption"));
        fCacheQuery[cache_goods_group] = QString("select f_id as `%1`, f_name as `%2` from c_groups")
                                         .arg(tr("Code"), tr("Name"));
        //fCacheQuery[cache_goods] = QString("select g.f_id as `%1`, gg.f_name as `%2`, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as `%3`, u.f_name as `%4`,
        fCacheQuery[cache_goods] = QString(R"(
                                            select g.f_id as `%1`, gg.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4`,
                                           g.f_scancode as `%5`, g.f_lastinputprice as `%6`, g.f_complectout as `%7`, g.f_qtybox as `%8`,
                                           gpr.f_price1 as `%9`, gpr.f_price2 as `%10`,
                                          if(length(coalesce(g.f_adg, ''))>0, g.f_adg, gg.f_adgcode)  as `%11`
                                           from c_goods g
                                           left join c_groups gg on gg.f_id=g.f_group
                                           left join c_units as u on u.f_id=g.f_unit
                                           left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1
                                        where 1=1 %idcond%
                                           order by 3
                                )")
                                   .arg(tr("Code").toLower())
                                   .arg(tr("Group").toLower())
                                   .arg(tr("Name").toLower())
                                   .arg(tr("Unit").toLower())
                                   .arg(tr("Scancode").toLower())
                                   .arg(tr("Price").toLower())
                                   .arg(tr("Complect output").toLower())
                                   .arg(tr("Qty in box").toLower())
                                   .arg(tr("Retail price").toLower())
                                   .arg(tr("Whosale price").toLower())
                                   .arg(tr("Adg code"));
        fCacheQuery[cache_goods_store] = QString("select f_id as `%1`, f_name as `%2` from c_storages")
                                         .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_goods_partners] =
            QString("select f_id as `%1`, f_name as `%2`, f_address as `%3`, f_taxname as `%4`, f_contact as `%5`, \
                                                    f_info as `%4`, f_phone `%5`, f_email as `%6`, f_taxcode as `%7` from c_partners ")
            .arg(tr("Code"))
            .arg(tr("Name"))
            .arg(tr("Address"))
            .arg(tr("Legal name"))
            .arg(tr("Contact"))
            .arg(tr("Info"))
            .arg(tr("Phone"))
            .arg(tr("Email"))
            .arg(tr("Taxcode"));
        fCacheQuery[cache_store_inout] = QString("select f_id as `%1`, f_name as `%2` from a_type_sign")
                                         .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_doc_state] = QString("select f_id as `%1`, f_name as `%2` from a_state")
                                       .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_doc_type] = QString("select f_id as `%1`, f_name as `%2` from a_type")
                                      .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_users] =
            QString("select f_id as `%1`, concat(f_last, ' ', f_first) as `%2`, f_teamlead from s_user where f_state=1")
            .arg(tr("Code"), tr("Name"), tr("Teamlead"));
        fCacheQuery[cache_waiter_printers] = QString("select f_id as `%1`, f_name as `%2` from d_printers")
                                             .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_credit_card] = QString("select f_id as `%1`, f_name as `%2` from o_credit_card")
                                         .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_dish_remove_reason] = QString("select f_id as `%1`, f_name as `%2` from o_dish_remove_reason")
                                                .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_s_db] = QString("select f_id as `%1`,  f_name as `%2`, f_description as `%3` from s_db")
                                  .arg(tr("Code"), tr("Name"), tr("Description"));
        setCacheSimpleQuery(cache_settings_names, "s_settings_names");
        setCacheSimpleQuery(cache_hall_list, "h_halls");
        setCacheSimpleQuery(cache_dish_comments, "d_dish_comment");
        setCacheSimpleQuery(cache_menu_names, "d_menu_names");
        setCacheSimpleQuery(cache_halls, "h_halls");
        setCacheSimpleQuery(cache_tables, "h_tables");
        setCacheSimpleQuery(cache_dish_package, "d_package");
        setCacheSimpleQuery(cache_salary_shift, "s_salary_shift");
        setCacheSimpleQuery(cache_goods_model, "c_goods_model");
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
        fCacheQuery[cache_cash_names] = QString("select f_id as `%1`, f_name as `%2`, f_currency as `%3` from e_cash_names")
                                        .arg(tr("Code"))
                                        .arg(tr("Name"))
                                        .arg(tr("Currency code"));
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
                                            .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_header_paid] = QString("select f_id as `%1`, f_name as `%2` from a_header_paid")
                                         .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_dish_menu_state] = QString("select f_id as `%1`, f_name as `%2` from d_dish_state")
                                             .arg(tr("Code"), tr("Name"));
        setCacheSimpleQuery(cache_order_mark, "b_marks");
        fCacheQuery[cache_mf_actions] = QString("select f_id as `%1`, f_name as `%2` from mf_actions ")
                                        .arg(tr("Code"), tr("Name"));
        fCacheQuery[cache_mf_process] =
            QString("select p.f_id as `%1`, p.f_rowid as `%2`, p.f_product as `%3`, gr.f_name as `%4`, "
                "p.f_process as `%5`, ac.f_name as `%6`, "
                "p.f_durationsec as `%7`, p.f_price as `%8` "
                "from mf_process p "
                "inner join mf_actions_group gr on gr.f_id=p.f_product "
                "inner join mf_actions ac on ac.f_id=p.f_process "
                "order by gr.f_name, p.f_rowid")
            .arg(tr("Code"), tr("Row"), tr("Product code"), tr("Product"), tr("Process code"), tr("Process"), tr("Duration"),
                 tr("Price"));
        setCacheSimpleQuery(cache_mf_products, "mf_actions_group");
        setCacheSimpleQuery(cache_mf_action_stage, "mf_actions_state");
        setCacheSimpleQuery(cache_mf_workshop, "mf_stage");
        fCacheQuery[cache_mf_active_task] = "select t.f_id, t.f_datecreate, p.f_name "
                                            "from mf_tasks t "
                                            "left join mf_actions_group p on p.f_id=t.f_product "
                                            "where t.f_state=1 ";
        setCacheSimpleQuery(cache_mf_task_state, "mf_tasks_state");
        fCacheQuery[cache_currency] =
            QString("select f_id as `%1`, f_name as `%2`, f_rate as `%3`, f_symbol as `%4` from e_currency ")
            .arg(tr("Code"), tr("Name"), tr("Rate"), tr("Symbol"));
        fCacheQuery[cache_currency_cross_rate] = QString("select cr.f_id as `%1`, cr.f_currency1 as `%2`, c1.f_name as `%3`, "
                                                         "cr.f_currency2 as `%4`, c2.f_name as `%5`, cr.f_rate as `%6` "
                                                         "from e_currency_cross_rate cr "
                                                         "left join e_currency c1 on c1.f_id=cr.f_currency1 "
                                                         "left join e_currency c2 on c2.f_id=cr.f_currency2 ")
            .arg(tr("Code"), tr("Currency code1"), tr("Currency name 1"),
                 tr("Currency code 2"), tr("Currency name 2"), tr("Rate"));
        fCacheQuery[cache_currency_cross_rate_history] =
            QString("select cr.f_id as `%1`, cr.f_date as `%2`, cr.f_currency1 as `%3`, c1.f_name as `%4`, "
                "cr.f_currency2 as `%5`, c2.f_name as `%6`, cr.f_rate as `%7` "
                "from e_currency_cross_rate_history cr "
                "left join e_currency c1 on c1.f_id=cr.f_currency1 "
                "left join e_currency c2 on c2.f_id=cr.f_currency2 ")
            .arg(tr("Code"), tr("Currency code1"), tr("Currency name 1"),
                 tr("Currency code 2"), tr("Currency name 2"), tr("Rate"));
        setCacheSimpleQuery(cache_draft_sale_state, "o_draft_sale_state");
        setCacheSimpleQuery(cache_partner_category, "c_partners_category");
        setCacheSimpleQuery(cache_partner_group, "c_partners_group");
        setCacheSimpleQuery(cache_partner_state, "c_partners_state");
        fCacheQuery[cache_materials_actions] = QString(R"(select g.f_id as `%1`, gg.f_name as `%2`, g.f_name as `%3`, u.f_name as `%4`,
                                                        g.f_scancode as `%5`, g.f_lastinputprice as `%6`, g.f_complectout as `%7`, g.f_qtybox as `%8`,
                                                        gpr.f_price1 as `%9`, gpr.f_price2 as `%10`
                                                        from c_goods g
                                                        left join c_groups gg on gg.f_id=g.f_group
                                                        left join c_units as u on u.f_id=g.f_unit
                                                        left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1
                                                        where g.f_production=1 and g.f_enabled=1
                                                        order by 3 )")
                                               .arg(tr("Code").toLower())
                                               .arg(tr("Group").toLower())
                                               .arg(tr("Name").toLower())
                                               .arg(tr("Unit").toLower())
                                               .arg(tr("Scancode").toLower())
                                               .arg(tr("Price").toLower())
                                               .arg(tr("Complect output").toLower())
                                               .arg(tr("Qty in box").toLower())
                                               .arg(tr("Retail price").toLower())
                                               .arg(tr("Whosale price").toLower());
        fCacheQuery[cache_goal_products] = QString(R"(
SELECT gp.f_id as `%1`, gp.f_date as `%2`, ms.f_name as `%3`, mf.f_name as `%4`
FROM m_goal_product gp
LEFT JOIN mf_actions_group mf ON mf.f_id=gp.f_product
LEFT JOIN m_goal_product_status ms ON ms.f_id=gp.f_status
)").arg(
                                               tr("Code"),
                                               tr("Date"),
                                               tr("Status"),
                                               tr("Product")
                                           );
    }

    if(fTableCache.count() == 0) {
        fTableCache["c_partners"] = cache_goods_partners;
        fTableCache["c_units"] = cache_goods_unit;
        fTableCache["c_groups"] = cache_goods_group;
        fTableCache["c_goods"] = cache_goods;
        fTableCache["d_part1"] = cache_dish_part1;
        fTableCache["d_part2"] = cache_dish_part2;
        fTableCache["d_dish"] = cache_dish;
        fTableCache["c_storages"] = cache_goods_store;
        fTableCache["s_user"] = cache_users;
        fTableCache["s_user_group"] = cache_users_groups;
        fTableCache["o_credit_card"] = cache_credit_card;
        fTableCache["o_dish_remove_reason"] = cache_dish_remove_reason;
        fTableCache["s_settings_names"] = cache_settings_names;
        fTableCache["h_halls"] = cache_hall_list;
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
        fTableCache["b_marks"] = cache_order_mark;
        fTableCache["mf_actions"] = cache_mf_actions;
        fTableCache["mf_process"] = cache_mf_process;
        fTableCache["mf_actions_group"] = cache_mf_products;
        fTableCache["mf_stage"] = cache_mf_workshop;
        fTableCache["mf_actions_state"] = cache_mf_action_stage;
        fTableCache["mf_tasks"] = cache_mf_active_task;
        fTableCache["mf_tasks_state"] = cache_mf_task_state;
        fTableCache["e_currency"] = cache_currency;
        fTableCache["e_currency_rate_history"] = cache_currency_rate;
        fTableCache["e_currency_cross_rate"] = cache_currency_cross_rate;
        fTableCache["e_currency_cross_rate_history"] = cache_currency_cross_rate_history;
        fTableCache["o_draft_sale_state"] = cache_draft_sale_state;
        fTableCache["c_partners_category"] = cache_partner_category;
        fTableCache["c_partners_group"] = cache_partner_group;
        fTableCache["c_partners_state"] = cache_partner_state;
        fTableCache["c_goods_model"] = cache_goods_model;
        fTableCache["cache_materials_actions"] = cache_materials_actions;
        fTableCache["cache_goal_products"] = cache_goal_products;
    }
}

QJsonArray C5Cache::getJoinedColumn(const QString &columnName)
{
    QJsonArray result;
    int columnIndex = fCacheColumns[fId][columnName.toLower()];

    for(int i = 0; i < fCacheData.size(); i++) {
        result.append(fCacheData.at(i).at(columnIndex));
    }

    return result;
}

QString C5Cache::getString(int id)
{
    int row = find(id);

    if(row > -1) {
        return getString(row, 1);
    } else {
        return "FIND ERROR";
    }
}

QString C5Cache::getString(int row, const QString &columnName)
{
#ifdef QT_DEBUG
    Q_ASSERT(fCacheColumns[fId].contains(columnName.toLower()));
#endif
    int col = fCacheColumns[fId][columnName.toLower()];
    return getString(row, col);
}

void C5Cache::refresh()
{
    loadFromDatabase(fCacheQuery[fId]);
}

void C5Cache::refreshId(const QString &whereField, int id)
{
    if(fCacheIdRow.contains(id)) {
        int row = fCacheIdRow[id];
        fCacheIdRow.remove(id);
        fCacheData.erase(fCacheData.begin() + row);
    }

    C5Database db;
    std::vector<QJsonArray > cacheData;
    QString sql = fCacheQuery[fId];
    db.exec(sql.replace("%idcond%", QString(" and %1=%2 ")
                        .arg(whereField, QString::number(id))), cacheData,
            fCacheColumns[fId]);

    if(cacheData.empty() == false) {
        fCacheData.push_back(cacheData.at(0));
        fCacheIdRow[id] = static_cast<int>(fCacheData.size()) - 1;
    }
}

C5Cache* C5Cache::cache(int cacheId)
{
    QString name = cacheName(cacheId);
    C5Cache *cache = nullptr;

    if(!fCacheList.contains(name)) {
        cache = new C5Cache();
        cache->fId = cacheId;
        fCacheList[name] = cache;
    } else {
        cache = fCacheList[name];
    }

    if(cache->fCacheData.size() == 0) {
        QString query = fCacheQuery[cacheId];

        if(query.isEmpty()) {
            query = "select 0";
        }

        cache->loadFromDatabase(query);
    }

    return cache;
}

QString C5Cache::cacheName(int cacheId)
{
    return QString::number(cacheId);
}

void C5Cache::resetCache(const QString &table)
{
    int cacheId = fTableCache[table];
    C5Cache *c = C5Cache::cache(cacheId);
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
    C5Database db;
    QString q = query;
    q.replace("%idcond%", "");
    db.exec(q, fCacheData, fCacheColumns[fId]);

    for(int i = 0; i < fCacheData.size(); i++) {
        fCacheIdRow[fCacheData[i][0].toInt()] = i;
    }
}

void C5Cache::setCacheSimpleQuery(int cacheId, const QString &table)
{
    fCacheQuery[cacheId] = QString("select f_id as `%1`, f_name as `%2` from %3 ")
                           .arg(tr("Code"), tr("Name"), table);
}
