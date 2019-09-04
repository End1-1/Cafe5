#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <QDateTime>

QString date_str(const QDate &d);
QString datetime_str(const QDateTime &d);
QString time_str(const QTime &t);

extern QString __format_date;
extern QString __format_datetime;
extern QString __format_time;

#endif // DATEUTILS_H
