#include "dict_payment_type.h"

const QVector<int> payment_types = {PAYMENT_TYPE_CASH,
                                    PAYMENT_TYPE_CARD,
                                    PAYMENT_TYPE_BANK,
                                    PAYMENT_TYPE_IDRAM,
                                    PAYMENT_TYPE_COMPLIMENTARY,
                                    PAYMENT_OTHER
                                   };

const QMap<int, QString> payment_fields = {
    {PAYMENT_TYPE_CASH, "f_amount_cash"},
    {PAYMENT_TYPE_CARD, "f_amount_card"},
    {PAYMENT_TYPE_BANK, "f_amount_bank"},
    {PAYMENT_TYPE_IDRAM, "f_amount_idram"},
    {PAYMENT_TYPE_COMPLIMENTARY, "f_amount_complimentary"},
    {PAYMENT_OTHER, "f_amount_other"}
};

const QMap<int, const char*> payment_names = {
    {PAYMENT_TYPE_CASH, QT_TRANSLATE_NOOP("PaymentType", "Cash")},
    {PAYMENT_TYPE_CARD, QT_TRANSLATE_NOOP("PaymentType", "Card")},
    {PAYMENT_TYPE_BANK, QT_TRANSLATE_NOOP("PaymentType", "Bank")},
    {PAYMENT_TYPE_IDRAM, QT_TRANSLATE_NOOP("PaymentType", "Idram")},
    {PAYMENT_TYPE_COMPLIMENTARY, QT_TRANSLATE_NOOP("PaymentType", "Complimentary")},
    {PAYMENT_OTHER, QT_TRANSLATE_NOOP("PaymentType", "Other")}
};

const QMap<int, bool> payment_special = {
    {PAYMENT_TYPE_CASH, false},
    {PAYMENT_TYPE_CARD, false},
    {PAYMENT_TYPE_BANK, true},
    {PAYMENT_TYPE_IDRAM, false},
    {PAYMENT_TYPE_COMPLIMENTARY, true},
    {PAYMENT_OTHER, true},
};

const QMap<int, bool> payment_mix = {
    {PAYMENT_TYPE_CASH, true},
    {PAYMENT_TYPE_CARD, true},
    {PAYMENT_TYPE_BANK, false},
    {PAYMENT_TYPE_IDRAM, true},
    {PAYMENT_TYPE_COMPLIMENTARY, false},
    {PAYMENT_OTHER, false},
};
