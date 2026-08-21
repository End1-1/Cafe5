#ifndef SHOPLOYALTYSTATE_H
#define SHOPLOYALTYSTATE_H

#include <QString>

/*
 * Состояние программы лояльности текущего заказа магазина.
 * Единственный источник правды для скидки/подарочной/накопительной карты.
 * UI-виджеты только отображают эти данные, но не хранят их.
 * Правило: подарочная и накопительная карты взаимоисключающие (gift XOR accumulate),
 * скидочная карта может действовать совместно с любой из них.
 */
struct ShopLoyaltyState {
    // Скидочная карта
    int discountCardId = 0;
    int discountPartnerId = 0;
    int discountType = 0;
    double discountFactor = 0;   // процент скидки

    // Подарочная карта
    int giftCardId = 0;
    double giftBalance = 0;

    // Накопительная карта
    int accumulateCardId = 0;
    double accumulateBalance = 0;
    double accumulatePercent = 0;

    // Сумма списания с активной карты (подарочная или накопительная)
    double useAmount = 0;

    bool hasDiscount() const { return discountCardId > 0 && discountFactor > 0.0001; }
    bool hasGift() const { return giftCardId > 0; }
    bool hasAccumulate() const { return accumulateCardId > 0; }

    double activeBalance() const
    {
        if (hasGift()) {
            return giftBalance;
        }
        if (hasAccumulate()) {
            return accumulateBalance;
        }
        return 0;
    }

    void clearDiscount()
    {
        discountCardId = 0;
        discountPartnerId = 0;
        discountType = 0;
        discountFactor = 0;
    }

    void clearGift()
    {
        giftCardId = 0;
        giftBalance = 0;
        useAmount = 0;
    }

    void clearAccumulate()
    {
        accumulateCardId = 0;
        accumulateBalance = 0;
        accumulatePercent = 0;
        useAmount = 0;
    }

    void clear()
    {
        clearDiscount();
        clearGift();
        clearAccumulate();
    }
};

#endif // SHOPLOYALTYSTATE_H
