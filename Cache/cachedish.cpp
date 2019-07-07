#include "cachedish.h"

QString query_cache_dish = QString("select d.f_id as `%1`, d.f_name as `%2`, p2.f_name as `%3` "
                                   "from d_dish d "
                                   "inner join d_part2 p2 on p2.f_id=d.f_part "
                                   "order by 3, 2 ")
        .arg(QObject::tr("Code"))
        .arg(QObject::tr("Name"))
        .arg(QObject::tr("Type"));
