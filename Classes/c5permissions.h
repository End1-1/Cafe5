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

#define cp_t2_action 200
#define cp_t2_store_input 201
#define cp_t2_store_output 202
#define cp_t2_store_move 203
#define cp_t2_store_inventory 204
#define cp_t2_count_output_of_sale 205
#define cp_t2_calculate_self_cost 206
#define cp_t2_store_complectation 207

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

#define cp_t6_goods_menu 600
#define cp_t6_units 601
#define cp_t6_groups 602
#define cp_t6_goods 603
#define cp_t6_waste 604
#define cp_t6_storage 605
#define cp_t6_partners 606
#define cp_t6_classes 607

#define cp_t7_other 700
#define cp_t7_credit_card 701
#define cp_t7_discount_system 702
#define cp_t7_upload_data_to_other_server 703
#define cp_t7_halls 704
#define cp_t7_tables 705
#define cp_t7_upload_date_settings 706
#define cp_t7_translator 707
#define cp_t7_store_reason 708
#define cp_t7_customers 709

#define cp_t8_cash 800
#define cp_t8_cash_doc 801
#define cp_t8_cash_common_report 802
#define cp_t8_cash_detailed_report 803
#define cp_t8_cash_names 804
#define cp_t8_costumer_debts 805
#define cp_t8_costumer_debts_pay 806

#define cp_t9_salary 900
#define cp_t9_salary_doc 901
#define cp_t9_report 902


class C5Permissions
{
public:
    C5Permissions();
    static QList<int> fTemplate;
    static void init(C5Database &db);
    static void clear();
    static QMap<QString, QList<int> > fPermissions;
};

bool pr(const QString &db, int permission);

#endif // C5PERMISSIONS_H
