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
#define FORMAT_DATE_TO_STR_MYSQL "yyyy-MM-dd"

#define current_date QDate::currentDate().toString(FORMAT_DATE_TO_STR)
#define current_time QTime::currentTime().toString(FORMAT_TIME_TO_STR)
#define hostinfo QHostInfo::localHostName().toLower()
#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))
#define str_float(value) QLocale().toDouble(value)

#define ORDER_STATE_NONE 0
#define ORDER_STATE_OPEN 1
#define ORDER_STATE_CLOSE 2
#define ORDER_STATE_VOID 3
#define ORDER_STATE_EMPTY 4

#define DISH_STATE_NONE 0
#define DISH_STATE_OK 1
#define DISH_STATE_MISTAKE 2
#define DISH_STATE_VOID 3

#define DOC_STATE_SAVED 1
#define DOC_STATE_DRAFT 2

#define DOC_TYPE_STORE_INPUT 1
#define DOC_TYPE_STORE_OUTPUT 2
#define DOC_TYPE_STORE_MOVE 3
#define DOC_TYPE_STORE_INVENTORY 4
#define DOC_TYPE_CASH 5
#define DOC_TYPE_COMPLECTATION 6
#define DOC_TYPE_SALARY 7
#define DOC_TYPE_DECOMPLECTATION 8
#define DOC_TYPE_SALE_INPUT 9

#define DOC_REASON_INPUT 1
#define DOC_REASON_MOVE 2
#define DOC_REASON_OUT 3
#define DOC_REASON_SALE 4
#define DOC_REASON_LOST 5
#define DOC_REASON_OVER 6
#define DOC_REASON_TRASH 7
#define DOC_REASON_COMPLECTATION 8
#define DOC_REASON_SALE_RETURN 9

#define CARD_TYPE_DISCOUNT 1
#define CARD_TYPE_MANUAL 2
#define CARD_TYPE_COUNT_ORDER 3
#define CARD_TYPE_ACCUMULATIVE 4
#define CARD_TYPE_AUTO_DISCOUNT 5

#define PAYOTHER_TRANSFER_TO_ROOM 1
#define PAYOTHER_COMPLIMENTARY 2
#define PAYOTHER_CL 3
#define PAYOTHER_SELFCOST 4
#define PAYOTHER_DEBT 5

#define SERVICE_AMOUNT_MODE_INCREASE_PRICE 1
#define SERVICE_AMOUNT_MODE_SEPARATE 2

#define TRANSLATOR_MENU_DISHES 1

class C5Utils
{
public:
    C5Utils();

    void writeErrorLog(const QString &text);

    QString pwd(const QString &value);
};

QString hostusername();

QString password(const QString &value);

extern C5Utils __c5utils;

#endif // C5UTILS_H
