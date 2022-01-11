#ifndef C5PERMISSIONS_H
#define C5PERMISSIONS_H

#include "c5database.h"
#include <QList>

#define cp_append_database 1

#define cp_t1_preference 100
#define cp_t1_usergroups 101
#define cp_t1_users 102
#define cp_t1_databases 103
#define cp_t1_settigns 104
#define cp_t1_login_to_manager 105
#define cp_t1_login_to_waiter 106
#define cp_t1_deny_change_cash_doc_date 107
#define cp_t1_deny_change_store_doc_date 108
#define cp_t1_goods_pricing 109

#define cp_t2_action 200
#define cp_t2_store_input 201
#define cp_t2_store_output 202
#define cp_t2_store_move 203
#define cp_t2_store_inventory 204
#define cp_t2_count_output_of_sale 205
#define cp_t2_calculate_self_cost 206
#define cp_t2_store_complectation 207
#define cp_t2_store_decomplectation 208

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
#define cp_t3_debts_to_partners 312
#define cp_t3_consuption_reason 313
#define cp_t3_car_visits 314
#define cp_t3_preorders 315
#define cp_t3_sale_effectiveness 316
#define cp_t3_storage_uncomplected 317
#define cp_t3_move_uncomplected 318
#define cp_t3_count_output_of_sale_draft 319
#define cp_t3_custom_reports 320
#define cp_t3_draft_output_recipes 321

#define cp_t4_menu 400
#define cp_t4_menu_names 401
#define cp_t4_part1 402
#define cp_t4_part2 403
#define cp_t4_dishes 404
#define cp_t4_dish_remove_reason 405
#define cp_t4_dish_comments 406
#define cp_t4_dish_price_self_cost 407
#define cp_t4_menu_review 408
#define cp_t4_dishes_packages 409

#define cp_t5_waiter 500
#define cp_t5_multiple_receipt 501
#define cp_t5_remove_printed_service 502
#define cp_t5_change_service_value 503
#define cp_t5_pay_transfer_to_room 504
#define cp_t5_pay_complimentary 505
#define cp_t5_pay_breakfast 506
#define cp_t5_pay_cityledger 507
#define cp_t5_refund_goods 508
#define cp_t5_shift_rotation 509
#define cp_t5_remove_row_from_shop 510
#define cp_t5_reports 511
#define cp_t5_view_reserved_tables 512
#define cp_t5_edit_reserve 513
#define cp_t5_movetable 514

#define cp_t5_enter_dlgface 515
#define cp_t5_change_staff_of_table 516
#define cp_t5_change_menu 517
#define cp_t5_stoplist 518
#define cp_t5_preorder 519
#define cp_t5_discount 520
#define cp_t5_cancel_discount 521
#define cp_t5_print_precheck 522
#define cp_t5_cancel_precheck 523
#define cp_t5_repeat_precheck 524
#define cp_t5_final_check 525
#define cp_t5_edit_table 526
#define cp_t5_splitguest 527
#define cp_t5_manager_tools 528
#define cp_t5_print_receipt 529
#define cp_t5_repeat_service 530
#define cp_t5_change_date_of_sale 531
#define cp_t5_bill_without_service 532
#define cp_t5_edit_closed_order 533
#define cp_t5_present 534
#define cp_t5_edit_booking 535
#define cp_t5_view_all_sales 536

#define cp_t5_pay_idram 537
#define cp_t5_pay_payx 538

#define cp_t6_goods_menu 600
#define cp_t6_units 601
#define cp_t6_groups 602
#define cp_t6_goods 603
#define cp_t6_waste 604
#define cp_t6_storage 605
#define cp_t6_classes 607
#define cp_t6_goods_images 608

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

#define cp_t8_cash 800
#define cp_t8_cash_doc 801
#define cp_t8_cash_common_report 802
#define cp_t8_cash_detailed_report 803
#define cp_t8_cash_names 804
#define cp_t8_costumer_debts 805
#define cp_t8_costumer_debts_pay 806
#define cp_t8_cash_movement 807

#define cp_t9_salary 900
#define cp_t9_salary_doc 901
#define cp_t9_report 902

#define cp_t10_manufacture 1000
#define cp_t10_action_list 1001
#define cp_t10_daily 1002
#define cp_t10_product_list 1003
#define cp_t10_general_report 1004


class C5Permissions
{
public:
    C5Permissions();
    static QList<int> fTemplate;
    static void init(C5Database &db, int group);
    static void clear();
    static QMap<QString, QList<int> > fPermissions;
};

#endif // C5PERMISSIONS_H
