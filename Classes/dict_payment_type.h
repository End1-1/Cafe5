#pragma once
#include <QObject>
#include <QVector>
#include <QMap>

#define PAYMENT_TYPE_CASH 1
#define PAYMENT_TYPE_CARD 2
#define PAYMENT_TYPE_BANK 3
#define PAYMENT_TYPE_IDRAM 4
#define PAYMENT_TYPE_COMPLIMENTARY 5
#define PAYMENT_OTHER 6

extern const QVector<int> payment_types;
extern const QMap<int, const char*> payment_names;
extern const QMap<int, QString> payment_fields;
extern const QMap<int, bool> payment_special;
extern const QMap<int, bool> payment_mix;
