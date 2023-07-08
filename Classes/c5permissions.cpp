#include "c5permissions.h"
#include "c5config.h"

QList<int> C5Permissions::fTemplate;
QMap<QString, QList<int> > C5Permissions::fPermissions;

C5Permissions::C5Permissions()
{

}

void C5Permissions::init(C5Database &db, int group)
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
              << cp_t1_allow_change_cash_doc_date
              << cp_t1_allow_change_store_doc_date
              << cp_t1_breeze

              << cp_t2_action
              << cp_t2_store_input
              << cp_t2_store_output
              << cp_t2_store_move
              << cp_t2_store_inventory
              << cp_t2_count_output_of_sale
              << cp_t2_calculate_self_cost
              << cp_t2_store_complectation
              << cp_t2_store_decomplectation
              << cp_t2_goods_reservations
              << cp_t2_reatail_trade
              << cp_t2_whosale_trade

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
              << cp_t3_debts
              << cp_t3_consuption_reason
              << cp_t3_preorders
              << cp_t3_sale_effectiveness
              << cp_t3_storage_uncomplected
              << cp_t3_move_uncomplected
              << cp_t3_count_output_of_sale_draft
              << cp_t3_custom_reports
              << cp_t3_draft_output_recipes

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
              << cp_t5_remove_row_from_shop
              << cp_t5_reports
              << cp_t5_view_reserved_tables
              << cp_t5_movetable
              << cp_t5_enter_dlgface
              << cp_t5_change_staff_of_table
              << cp_t5_change_menu
              << cp_t5_stoplist
              << cp_t5_preorder
              << cp_t5_discount
              << cp_t5_cancel_discount
              << cp_t5_print_precheck
              << cp_t5_cancel_precheck
              << cp_t5_repeat_precheck
              << cp_t5_final_check
              << cp_t5_edit_table
              << cp_t5_splitguest
              << cp_t5_manager_tools
              << cp_t5_print_receipt
              << cp_t5_repeat_service
              << cp_t5_change_date_of_sale
              << cp_t5_bill_without_service
              << cp_t5_edit_closed_order
              << cp_t5_present
              << cp_t5_edit_booking
              << cp_t5_pay_idram
              << cp_t5_pay_payx
              << cp_t5_view_tax_and_no_sales
              << cp_t5_view_sales_of_all_users

              << cp_t6_storage
              << cp_t6_goods_menu
              << cp_t6_goods
              << cp_t6_units
              << cp_t6_groups
              << cp_t6_waste
              << cp_t6_classes
              << cp_t6_goods_images
              << cp_t6_qty_reminder
              << cp_t6_complectations
              << cp_t6_goods_special_prices

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
              << cp_t7_order_marks
              << cp_t7_route
              << cp_t7_route_exec

              << cp_t8_cash
              << cp_t8_cash_doc
              << cp_t8_cash_common_report
              << cp_t8_cash_detailed_report
              << cp_t8_cash_names
              << cp_t8_cash_movement
              << cp_t8_currency
              << cp_t8_edit_currency
              << cp_t8_currency_cross_rate
              << cp_t8_currency_cross_rate_history

              << cp_t9_salary
              << cp_t9_salary_doc
              << cp_t9_report
              << cp_t9_payment

              << cp_t10_manufacture
              << cp_t10_action_list
              << cp_t10_daily
              << cp_t10_product_list
              << cp_t10_general_report
              << cp_t10_general_report_only_date
              << cp_t10_actions_stages
              << cp_t10_workshops
              << cp_t10_active_tasks
              << cp_t10_create_task
                 ;
    if (group == 1) {
        db[":f_group"] = 1;
        db.exec("delete from s_user_access where f_group=:f_group");
        QString sql = "insert into s_user_access (f_group, f_key, f_value) values ";
        bool f = true;
        for (auto i: fTemplate) {
            if (f) {
                f = false;
            } else {
                sql += ",";
            }
            sql += QString("(1, %1, 1)").arg(i);
        }
        db.exec(sql);
    }
    db[":f_group"] = group;
    db.exec("select f_key from s_user_access where f_group=:f_group");
    fPermissions.clear();
    while (db.nextRow()) {
        fPermissions[db.database()] << db.getInt(0);
    }
}

void C5Permissions::clear()
{
    fPermissions.clear();
    fTemplate.clear();
}
