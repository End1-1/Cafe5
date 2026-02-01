#ifndef C5PERMISSIONS_H
#define C5PERMISSIONS_H

#include <QList>

#define cp_append_database 1

#define cp_t1_preference 100
#define cp_t1_usergroups 101
#define cp_t1_users 102
#define cp_t1_databases 103
#define cp_t1_settigns 104
#define cp_t1_login_to_manager 105
#define cp_t1_login_to_waiter 106
#define cp_t1_allow_change_cash_doc_date 107
#define cp_t1_allow_change_store_doc_date 108
#define cp_t1_goods_pricing 109
#define cp_t1_breeze 110

#define cp_t2_action 200
#define cp_t2_store_input 201
#define cp_t2_store_output 202
#define cp_t2_store_move 203
#define cp_t2_store_inventory 204
#define cp_t2_count_output_of_sale 205
#define cp_t2_calculate_self_cost 206
#define cp_t2_store_complectation 207
#define cp_t2_store_decomplectation 208
#define cp_t2_goods_reservations 209
#define cp_t2_reatail_trade 210
#define cp_t2_whosale_trade 211

#define cp_t3_reports 300
#define cp_t3_sales_common 301
#define cp_t3_documents 302
#define cp_t3_store 303
#define cp_t3_store_movement 304
#define cp_t3_tstore_extra 305
#define cp_t3_store_sale 306
#define cp_t3_documents_store 307
#define cp_t3_sale_removed_dishes 308
#define cp_t3_sale_dishes 309
#define cp_t3_sale_from_store_total 310
#define cp_t3_discount_statistics 311
#define cp_t3_debts_partner 312
#define cp_t3_consuption_reason 313
#define cp_t3_preorders 315
#define cp_t3_storage_uncomplected 317
#define cp_t3_move_uncomplected 318
#define cp_t3_custom_reports 320
#define cp_t3_draft_output_recipes 321
#define cp_t3_debts_customer 322

#define cp_t4_menu 400
#define cp_t4_menu_names 401
#define cp_t4_part1 402
#define cp_t4_part2 403
#define cp_t4_dish_remove_reason 405
#define cp_t4_menu_review 408
#define cp_t4_dishes_packages 409

#define cp_t5_waiter 500
#define cp_t5_waiter_edit_order 501
#define cp_t5_waiter_print_precheck 502
#define cp_t5_waiter_reprint_prechek 503
#define cp_t5_waiter_cancel_precheck 504
#define cp_t5_waiter_close_order 505
#define cp_t5_waiter_remove_printed_goods 506
#define cp_t5_waiter_transfer_items 507
#define cp_t5_waiter_special_payment_types 508
#define cp_t5_waiter_change_service_factor 509
#define cp_t5_waiter_remove_discount 510
#define cp_t5_waiter_open_close_shift 511
#define cp_t5_waiter_reports 512
#define cp_t5_waiter_edit_stoplist 513
#define cp_t5_waiter_reprint_goods 514

#define cp_t6_goods_menu 600
#define cp_t6_units 601
#define cp_t6_groups 602
#define cp_t6_goods 603
#define cp_t6_storage 605
#define cp_t6_goods_images 608
#define cp_t6_qty_reminder 609
#define cp_t6_complectations 610
#define cp_t6_goods_only_price_edit 611
#define cp_t6_goods_special_prices 612
#define cp_t6_goods_price 613

#define cp_t7_other 700
#define cp_t7_credit_card 701
#define cp_t7_discount_system 702
#define cp_t7_upload_data_to_other_server 703
#define cp_t7_halls 704
#define cp_t7_tables 705
#define cp_t7_upload_date_settings 706
#define cp_t7_translator 707
#define cp_t7_store_reason 708
#define cp_t7_partners 709
#define cp_t7_order_marks 710
#define cp_t7_route 711
#define cp_t7_route_exec 712

#define cp_t8_cash 800
#define cp_t8_cash_doc 801
#define cp_t8_cash_common_report 802
#define cp_t8_cash_detailed_report 803
#define cp_t8_cash_names 804
#define cp_t8_cash_movement 807
#define cp_t8_currency 808
#define cp_t8_edit_currency 809
#define cp_t8_currency_cross_rate 810
#define cp_t8_currency_cross_rate_history 811
#define cp_t8_shifts 812

#define cp_t9_salary 900
#define cp_t9_salary_doc 901
#define cp_t9_report 902
#define cp_t9_payment 903

#define cp_t10_manufacture 1000
#define cp_t10_action_list 1001
#define cp_t10_daily 1002
#define cp_t10_product_list 1003
#define cp_t10_general_report 1004
#define cp_t10_general_report_only_date 1005
#define cp_t10_workshops 1006
#define cp_t10_actions_stages 1007
#define cp_t10_active_tasks 1008
#define cp_t10_create_task 1009

#define cp_t12_shop_enter_sale 1203
#define cp_t12_shop_enter_store 1204
#define cp_t12_shop_fiscal_report 1205
#define cp_t12_shop_sale_of_all_users 1206
#define cp_t12_shop_discount 1207
#define cp_t12_shop_report_goods 1208
#define cp_t12_remove_order_row 1209

#endif // C5PERMISSIONS_H
