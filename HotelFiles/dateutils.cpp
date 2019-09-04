#include "dateutils.h"

QString __format_date = "dd/MM/yyyy";
QString __format_datetime = "dd/MM/yyyy HH:mm:ss";
QString __format_time = "HH:mm:ss";

QString date_str(const QDate &d)
{
    return d.toString(__format_date);
}

QString datetime_str(const QDateTime &d)
{
    return d.toString(__format_datetime);
}

QString time_str(const QTime &t)
{
    return t.toString(__format_time);
}
