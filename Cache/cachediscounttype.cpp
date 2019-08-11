#include "cachediscounttype.h"


QString query_discount_type = QString("select f_id as `%1`, f_name as `%2` from b_card_types")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
