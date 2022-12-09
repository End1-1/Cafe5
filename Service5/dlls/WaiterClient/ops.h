#ifndef OPS_H
#define OPS_H

#include "rawmessage.h"

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

#define version1 1 //jazzve
#define version2 2 //cafe5
#define version3 3 //elite

bool rmerror(RawMessage &rm, const QString &err);

#endif // OPS_H
