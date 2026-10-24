#pragma once
#include <QObject>
#include <QVector>
#include <QMap>

#define PAYMENT_TYPE_CASH 1
#define PAYMENT_TYPE_CARD 2
#define PAYMENT_TYPE_BANK 3
#define PAYMENT_TYPE_IDRAM 4
#define PAYMENT_TYPE_COMPLIMENTARY 5
#define PAYMENT_TYPE_OTHER 6
#define PAYMENT_TYPE_TELCELL 7
#define PAYMENT_TYPE_DEBT 8
#define PAYMENT_TYPE_PREPAID 9
/** Extra bank transfer accounts: purchases, debts and cash operations only (not sale tenders). */
#define PAYMENT_TYPE_BANK2 10
#define PAYMENT_TYPE_BANK3 11

extern const QVector<int> payment_types;
extern const QMap<int, const char*> payment_names;
extern const QMap<int, QString> payment_fields;
extern const QMap<int, bool> payment_special;
extern const QMap<int, bool> payment_mix;
