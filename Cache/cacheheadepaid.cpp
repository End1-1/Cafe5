#include "cacheheadepaid.h"

QString query_cache_header_paid = QString("select f_id as `%1`, f_name as `%2` from a_header_paid")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
