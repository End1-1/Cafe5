#include "dict_payment_type.h"

const QVector<int> payment_types = {PAYMENT_TYPE_CASH,
                                    PAYMENT_TYPE_CARD,
                                    PAYMENT_TYPE_BANK,
                                    PAYMENT_TYPE_IDRAM,
                                    PAYMENT_TYPE_COMPLIMENTARY
                                   };
const QMap<int, const char*> payment_names = {
    {PAYMENT_TYPE_CASH, QT_TRANSLATE_NOOP("PaymentType", "Cash")},
    {PAYMENT_TYPE_CARD, QT_TRANSLATE_NOOP("PaymentType", "Card")},
    {PAYMENT_TYPE_BANK, QT_TRANSLATE_NOOP("PaymentType", "Bank")},
    {PAYMENT_TYPE_IDRAM, QT_TRANSLATE_NOOP("PaymentType", "Idram")},
    {PAYMENT_TYPE_COMPLIMENTARY, QT_TRANSLATE_NOOP("PaymentType", "Complimentary")}
};
const QMap<int, QString> payment_fields = {
    {PAYMENT_TYPE_CASH, "f_amount_cash"},
    {PAYMENT_TYPE_CARD, "f_amount_card"},
    {PAYMENT_TYPE_BANK, "f_amount_bank"},
    {PAYMENT_TYPE_IDRAM, "f_amount_idram"},
    {PAYMENT_TYPE_IDRAM, "f_amount_complimentary"}
};
