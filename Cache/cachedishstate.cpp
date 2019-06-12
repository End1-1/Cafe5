#include "cachedishstate.h"

QString query_cache_dish_state = QString("select f_id as `%1`, f_name as `%2` from o_body_state")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
