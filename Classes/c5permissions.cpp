#include "c5permissions.h"
#include "c5config.h"

QList<int> C5Permissions::fTemplate;
QMap<QString, QList<int> > C5Permissions::fPermissions;

C5Permissions::C5Permissions()
{

}

void C5Permissions::init(C5Database &db)
{
    clear();
    fTemplate << cp_append_database

              << cp_t1_preference
              << cp_t1_usergroups
              << cp_t1_users
              << cp_t1_databases
              << cp_t1_settigns

              << cp_t2_action
              << cp_t2_store_input
              << cp_t2_store_output
              << cp_t2_store_move
              << cp_t2_store_inventory
              << cp_t2_count_output_of_sale

              << cp_t3_reports
              << cp_t3_sales_common
              << cp_t3_documents
              << cp_t3_store
              << cp_t3_store_movement
              << cp_t3_tstore_extra

              << cp_t4_menu
              << cp_t4_menu_names
              << cp_t4_part1
              << cp_t4_part2
              << cp_t4_dishes
              << cp_t4_dish_remove_reason
              << cp_t4_dish_comments

              << cp_t5_waiter
              << cp_t5_multiple_receipt
              << cp_t5_remove_printed_service

              << cp_t6_storage
              << cp_t6_goods_menu
              << cp_t6_goods
              << cp_t6_units
              << cp_t6_groups
              << cp_t6_waste
              << cp_t6_partners

              << cp_t7_other
              << cp_t7_credit_card
              << cp_t7_discount_system
                 ;
    if (__usergroup == 1) {
        fPermissions[db.database()] = fTemplate;
        return;
    }
    db[":f_group"] = __usergroup;
    db.exec("select f_key from s_user_access where f_group=:f_group");
    while (db.nextRow()) {
        fPermissions[db.database()] << db.getInt(0);
    }
}

void C5Permissions::clear()
{
    fPermissions.clear();
    fTemplate.clear();
}

bool pr(const QString &db, int permission)
{
    return C5Permissions::fPermissions[db].contains(permission);
}
