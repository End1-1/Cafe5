#ifndef FORMAT_DATE_H
#define FORMAT_DATE_H

#define FORMAT_DATETIME_TO_STR "dd/MM/yyyy HH:mm:ss"
#define FORMAT_DATETIME_TO_STR_MYSQL "yyyy-MM-dd HH:mm:ss"
#define FORMAT_DATETIME_TO_STR2 "dd/MM/yyyy HH:mm"
#define FORMAT_DATE_TO_STR "dd/MM/yyyy"
#define FORMAT_TIME_TO_STR "HH:mm:ss"
#define FORMAT_TIME_TO_SHORT_STR "HH:mm"
#define FORMAT_DATE_TO_STR_MYSQL "yyyy-MM-dd"

#define current_date QDate::currentDate().toString(FORMAT_DATE_TO_STR)
#define current_time QTime::currentTime().toString(FORMAT_TIME_TO_STR)
#define str_to_datetime(value) QDateTime::fromString(value, FORMAT_DATETIME_TO_STR_MYSQL)

#endif // FORMAT_DATE_H
