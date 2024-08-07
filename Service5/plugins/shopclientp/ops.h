#ifndef OPS_H
#define OPS_H

#include "rawmessage.h"
#include <QRegExp>

#define op_login 1
#define op_login_passhash 2
#define op_check_qty 3
#define op_data_currency_list 4
#define op_data_currency_crossrate_list 5
#define op_get_goods_list 6
#define op_goods_prices 7
#define op_create_empty_sale 8
#define op_open_sale_draft_document 9
#define op_show_drafts_sale_list 10
#define op_add_goods_to_draft 11
#define op_open_sale_draft_body 12
#define op_udpdate_draft_header 13

#define op_json_partners_list 100
#define op_json_predefined_goods 101
#define op_json_debts 102
#define op_json_storagenames 103
#define op_json_stock 104
#define op_json_sales_history 105
#define op_json_route 106
#define op_json_route_update 107
#define op_json_stock_all 108
#define op_json_partner_debt 109

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
