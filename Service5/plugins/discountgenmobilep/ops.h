#ifndef OPS_H
#define OPS_H

#include "rawmessage.h"
#include <QRegExp>

#define op_login 1
#define op_login_passhash 2
#define op_create_qr_discount 3
#define op_check_bonuses 4

#define float_str(value, f) QString::number(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))
#define FORMAT_DATETIME_TO_STR "dd.MM.yyyy HH:mm:ss"
#define FORMAT_DATE_TO_STR "dd.MM.yyyy"
#define FORMAT_TIME_TO_STR "HH:mm:ss"

#define ORDER_STATE_PREORDER_EMPTY 5
#define ORDER_STATE_PREORDER_WITH_ORDER 6

bool rmerror(RawMessage &rm, const QString &err);

#endif // OPS_H
