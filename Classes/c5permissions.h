#ifndef C5PERMISSIONS_H
#define C5PERMISSIONS_H

#include <QList>

const int cp_append_database = 1;

const int cp_t1_preference = 100;
const int cp_t1_usergroups = 101;
const int cp_t1_users = 102;
const int cp_t1_databases = 103;
const int cp_t1_settigns = 104;
const int cp_t1_login_to_manager = 105;
const int cp_t1_login_to_waiter = 106;
const int cp_t1_allow_change_cash_doc_date = 107;
const int cp_t1_allow_change_store_doc_date = 108;
const int cp_t1_goods_pricing = 109;
const int cp_t1_breeze = 110;

const int cp_t2_action = 200;
const int cp_t2_store_inventory = 204;
const int cp_t2_calculate_self_cost = 206;
const int cp_t2_goods_reservations = 209;
const int cp_t2_reatail_trade = 210;
const int cp_t2_whosale_trade = 211;

const int cp_t3_reports = 300;
const int cp_t3_store = 303;
const int cp_t3_store_sale = 306;
const int cp_t3_sale_removed_dishes = 308;
const int cp_t3_sale_dishes = 309;
const int cp_t3_sale_from_store_total = 310;
const int cp_t3_discount_statistics = 311;
const int cp_t3_debts_partner = 312;
const int cp_t3_consuption_reason = 313;
const int cp_t3_preorders = 315;
const int cp_t3_storage_uncomplected = 317;
const int cp_t3_move_uncomplected = 318;
const int cp_t3_custom_reports = 320;
const int cp_t3_debts_customer = 322;

const int cp_t4_menu = 400;
const int cp_t4_menu_names = 401;
const int cp_t4_part1 = 402;
const int cp_t4_part2 = 403;
const int cp_t4_dish_remove_reason = 405;
const int cp_t4_menu_review = 408;
const int cp_t4_dishes_packages = 409;

const int cp_t5_waiter = 500;
const int cp_t5_waiter_edit_order = 501;
const int cp_t5_waiter_print_precheck = 502;
const int cp_t5_waiter_reprint_prechek = 503;
const int cp_t5_waiter_cancel_precheck = 504;
const int cp_t5_waiter_close_order = 505;
const int cp_t5_waiter_remove_printed_goods = 506;
const int cp_t5_waiter_transfer_items = 507;
const int cp_t5_waiter_special_payment_types = 508;
const int cp_t5_waiter_change_service_factor = 509;
const int cp_t5_waiter_remove_discount = 510;
const int cp_t5_waiter_open_close_shift = 511;
const int cp_t5_waiter_reports = 512;
const int cp_t5_waiter_edit_stoplist = 513;
const int cp_t5_waiter_reprint_goods = 514;
const int cp_t5_waiter_cash_operations = 515;

const int cp_t6_goods_menu = 600;
const int cp_t6_units = 601;
const int cp_t6_groups = 602;
const int cp_t6_goods = 603;
const int cp_t6_storage = 605;
const int cp_t6_goods_images = 608;
const int cp_t6_qty_reminder = 609;
const int cp_t6_complectations = 610;
const int cp_t6_goods_only_price_edit = 611;
const int cp_t6_goods_special_prices = 612;
const int cp_t6_goods_price = 613;

const int cp_t7_other = 700;
const int cp_t7_credit_card = 701;
const int cp_t7_discount_system = 702;
const int cp_t7_upload_data_to_other_server = 703;
const int cp_t7_halls = 704;
const int cp_t7_tables = 705;
const int cp_t7_upload_date_settings = 706;
const int cp_t7_translator = 707;
const int cp_t7_store_reason = 708;
const int cp_t7_partners = 709;
const int cp_t7_order_marks = 710;
const int cp_t7_route = 711;
const int cp_t7_route_exec = 712;

const int cp_t8_cash = 800;
const int cp_t8_cash_doc = 801;
const int cp_t8_cash_common_report = 802;
const int cp_t8_cash_detailed_report = 803;
const int cp_t8_cash_names = 804;
const int cp_t8_cash_movement = 807;
const int cp_t8_currency = 808;
const int cp_t8_edit_currency = 809;
const int cp_t8_currency_cross_rate = 810;
const int cp_t8_currency_cross_rate_history = 811;
const int cp_t8_shifts = 812;

const int cp_t9_salary = 900;
const int cp_t9_salary_doc = 901;
const int cp_t9_report = 902;
const int cp_t9_payment = 903;

const int cp_t10_manufacture = 1000;
const int cp_t10_action_list = 1001;
const int cp_t10_daily = 1002;
const int cp_t10_product_list = 1003;
const int cp_t10_general_report = 1004;
const int cp_t10_general_report_only_date = 1005;
const int cp_t10_workshops = 1006;
const int cp_t10_actions_stages = 1007;
const int cp_t10_active_tasks = 1008;
const int cp_t10_create_task = 1009;

const int cp_t12_shop_enter_sale = 1203;
const int cp_t12_shop_enter_store = 1204;
const int cp_t12_shop_fiscal_report = 1205;
const int cp_t12_shop_sale_of_all_users = 1206;
const int cp_t12_shop_discount = 1207;
const int cp_t12_shop_report_goods = 1208;
const int cp_t12_remove_order_row = 1209;

#endif // C5PERMISSIONS_H
