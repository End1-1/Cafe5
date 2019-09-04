#include "cacheheaderpayment.h"

QString query_cache_header_payment = QString("select f_id as `%1`, f_name as `%2` from a_header_payment")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
