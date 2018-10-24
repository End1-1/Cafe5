#ifndef C5UTILS_H
#define C5UTILS_H

#include <QString>
#include <QHostInfo>
#include <QDateTime>
#include <QCryptographicHash>

#define FORMAT_DATETIME_TO_STR "dd/MM/yyyy HH:mm:ss"
#define FORMAT_DATETIME_TO_STR2 "dd/MM/yyyy HH:mm"
#define FORMAT_DATE_TO_STR "dd/MM/yyyy"
#define FORMAT_TIME_TO_STR "HH:mm:ss"

#define current_date QDate::currentDate().toString(FORMAT_DATE_TO_STR)
#define current_time QTime::currentTime().toString(FORMAT_TIME_TO_STR)
#define hostinfo QHostInfo::localHostName().toLower()
#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))

#define ORDER_STATE_NONE 0
#define ORDER_STATE_OPEN 1
#define ORDER_STATE_CLOSE 2
#define ORDER_STATE_VOID 3

#define DISH_STATE_NONE 0
#define DISH_STATE_OK 1
#define DISH_STATE_MISTAKE 2
#define DISH_STATE_VOID 3

class C5Utils
{
public:
    C5Utils();

    void writeErrorLog(const QString &text);

    QString pwd(const QString &value);
};

QString password(const QString &value);

extern C5Utils __c5utils;

#endif // C5UTILS_H
