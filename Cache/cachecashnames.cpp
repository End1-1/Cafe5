#include "cachecashnames.h"

QString query_cache_cash_names = QString("select f_id as `%1`, f_name as `%2` from e_cash_names")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"));
