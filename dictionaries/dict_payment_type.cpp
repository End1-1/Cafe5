#include "dict_payment_type.h"

const QVector<int> payment_types = {PAYMENT_TYPE_CASH,
                                    PAYMENT_TYPE_CARD,
                                    PAYMENT_TYPE_BANK,
                                    PAYMENT_TYPE_IDRAM,
                                    PAYMENT_TYPE_COMPLIMENTARY,
                                    PAYMENT_TYPE_OTHER,
                                    PAYMENT_TYPE_TELCELL,
                                    PAYMENT_TYPE_DEBT,
                                    PAYMENT_TYPE_PREPAID};

const QMap<int, QString> payment_fields = {{PAYMENT_TYPE_CASH, "f_amount_cash"},
                                           {PAYMENT_TYPE_CARD, "f_amount_card"},
                                           {PAYMENT_TYPE_BANK, "f_amount_bank"},
                                           {PAYMENT_TYPE_IDRAM, "f_amount_idram"},
                                           {PAYMENT_TYPE_COMPLIMENTARY, "f_amount_complimentary"},
                                           {PAYMENT_TYPE_OTHER, "f_amount_other"},
                                           {PAYMENT_TYPE_TELCELL, "f_amount_telcell"},
                                           {PAYMENT_TYPE_DEBT, "f_amount_debt"},
                                           {PAYMENT_TYPE_PREPAID, "f_amount_prepaid"}};

const QMap<int, const char *> payment_names = {{PAYMENT_TYPE_CASH, QT_TRANSLATE_NOOP("PaymentType", "Cash")},
                                               {PAYMENT_TYPE_CARD, QT_TRANSLATE_NOOP("PaymentType", "Card")},
                                               {PAYMENT_TYPE_BANK, QT_TRANSLATE_NOOP("PaymentType", "Bank")},
                                               {PAYMENT_TYPE_IDRAM, QT_TRANSLATE_NOOP("PaymentType", "Idram")},
                                               {PAYMENT_TYPE_COMPLIMENTARY, QT_TRANSLATE_NOOP("PaymentType", "Complimentary")},
                                               {PAYMENT_TYPE_OTHER, QT_TRANSLATE_NOOP("PaymentType", "Other")},
                                               {PAYMENT_TYPE_TELCELL, QT_TRANSLATE_NOOP("PaymentType", "TelCell")},
                                               {PAYMENT_TYPE_DEBT, QT_TRANSLATE_NOOP("PaymentType", "Debt")},
                                               {PAYMENT_TYPE_PREPAID, QT_TRANSLATE_NOOP("PaymentType", "Prepaid")}};

const QMap<int, bool> payment_special = {
    {PAYMENT_TYPE_CASH, false},
    {PAYMENT_TYPE_CARD, false},
    {PAYMENT_TYPE_BANK, true},
    {PAYMENT_TYPE_IDRAM, false},
    {PAYMENT_TYPE_COMPLIMENTARY, true},
    {PAYMENT_TYPE_OTHER, true},
    {PAYMENT_TYPE_TELCELL, true},
    {PAYMENT_TYPE_DEBT, true},
    {PAYMENT_TYPE_PREPAID, false},
};

const QMap<int, bool> payment_mix = {
    {PAYMENT_TYPE_CASH, true},
    {PAYMENT_TYPE_CARD, true},
    {PAYMENT_TYPE_BANK, false},
    {PAYMENT_TYPE_IDRAM, true},
    {PAYMENT_TYPE_COMPLIMENTARY, false},
    {PAYMENT_TYPE_OTHER, false},
    {PAYMENT_TYPE_TELCELL, false},
    {PAYMENT_TYPE_DEBT, false},
    {PAYMENT_TYPE_PREPAID, true},
};
