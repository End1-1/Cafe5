#ifndef OPS_H
#define OPS_H

#include "rawmessage.h"
#include <QRegExp>

#define op_login 1
#define op_get_hall_list 2
#define op_get_table_list 3
#define op_get_dish_part1_list 4
#define op_get_dish_part2_list 5
#define op_get_dish_dish_list 6
#define op_login_passhash 7
#define op_open_table 8
#define op_unlock_table 9
#define op_car_list 10
#define op_search_platenumber 11
#define op_create_order_header 12
#define op_set_car 13
#define op_open_order 14
#define op_get_car 15
#define op_dish_menu 16
#define op_add_dish_to_order 17
#define op_remove_dish_from_order 18
#define op_modify_order_dish 19
#define op_print_service 20
#define op_login_pin 21
#define op_update_tables 22
#define op_get_dish_comment 23
#define op_print_bill 24
#define op_ready_dishes 25
#define op_get_menu_list 26

#define version1 1 //jazzve
#define version2 2 //cafe5
#define version3 3 //elite

#define float_str(value, f) QString::number(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))
#define FORMAT_DATETIME_TO_STR "dd.MM.yyyy HH:mm:ss"
#define FORMAT_DATE_TO_STR "dd.MM.yyyy"
#define FORMAT_TIME_TO_STR "HH:mm:ss"

#define ORDER_STATE_PREORDER_EMPTY 5
#define ORDER_STATE_PREORDER_WITH_ORDER 6

bool rmerror(RawMessage &rm, const QString &err);

#endif // OPS_H
