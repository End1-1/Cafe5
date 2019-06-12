#include "cachestorereason.h"

QString query_cache_store_reason = QString("select f_id as `%1`, f_name as `%2` from a_reason")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
