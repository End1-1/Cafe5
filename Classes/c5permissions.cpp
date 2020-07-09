#include "c5permissions.h"
#include "c5config.h"

QList<int> C5Permissions::fTemplate;
QMap<QString, QList<int> > C5Permissions::fPermissions;

C5Permissions::C5Permissions()
{

}

void C5Permissions::init(C5Database &db)
{
    fPermissions[db.database()].clear();
    fTemplate << cp_append_database

              << cp_t1_preference
              << cp_t1_usergroups
              << cp_t1_users
              << cp_t1_databases
              << cp_t1_settigns
              << cp_t1_login_to_manager
              << cp_t1_login_to_waiter
              << cp_t1_goods_pricing

              << cp_t2_action
              << cp_t2_store_input
              << cp_t2_store_output
              << cp_t2_store_move
              << cp_t2_store_inventory
              << cp_t2_count_output_of_sale
              << cp_t2_calculate_self_cost
              << cp_t2_store_complectation
              << cp_t2_store_decomplectation

              << cp_t3_reports
              << cp_t3_sales_common
              << cp_t3_documents
              << cp_t3_store
              << cp_t3_store_movement
              << cp_t3_tstore_extra
              << cp_t3_store_sale
              << cp_t3_documents_store
              << cp_t3_sale_removed_dishes
              << cp_t3_sale_dishes
              << cp_t3_sale_from_store_total
              << cp_t3_discount_statistics
              << cp_t3_debts_to_partners
              << cp_t3_consuption_reason
              << cp_t3_car_visits
              << cp_t3_preorders
              << cp_t3_sale_effectiveness

              << cp_t4_menu
              << cp_t4_menu_names
              << cp_t4_part1
              << cp_t4_part2
              << cp_t4_dishes
              << cp_t4_dish_remove_reason
              << cp_t4_dish_comments
              << cp_t4_dish_price_self_cost
              << cp_t4_menu_review
              << cp_t4_dishes_packages

              << cp_t5_waiter
              << cp_t5_multiple_receipt
              << cp_t5_remove_printed_service
              << cp_t5_change_service_value
              << cp_t5_pay_transfer_to_room
              << cp_t5_pay_cityledger
              << cp_t5_pay_breakfast
              << cp_t5_pay_complimentary
              << cp_t5_refund_goods
              << cp_t5_shift_rotation

              << cp_t6_storage
              << cp_t6_goods_menu
              << cp_t6_goods
              << cp_t6_units
              << cp_t6_groups
              << cp_t6_waste
              << cp_t6_classes
              << cp_t6_goods_images

              << cp_t7_other
              << cp_t7_credit_card
              << cp_t7_discount_system
              << cp_t7_upload_data_to_other_server
              << cp_t7_halls
              << cp_t7_tables
              << cp_t7_upload_date_settings
              << cp_t7_translator
              << cp_t7_store_reason
              << cp_t7_partners

              << cp_t8_cash
              << cp_t8_cash_doc
              << cp_t8_cash_common_report
              << cp_t8_cash_detailed_report
              << cp_t8_cash_names
              << cp_t8_costumer_debts
              << cp_t8_costumer_debts_pay
              << cp_t8_cash_movement

              << cp_t9_salary
              << cp_t9_salary_doc
              << cp_t9_report
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
    QList<int> &p = C5Permissions::fPermissions[db];
    return p.contains(permission);
}
