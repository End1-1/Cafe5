#pragma once

#include "menutypes.h"

#include <QHash>

class OrderCart
{
public:
    void clear();
    void addDish(const MenuDish &dish);

    int itemCount() const;
    double totalAmount() const;

private:
    QHash<int, MenuDish> m_dishes;
    QHash<int, int> m_qty;
};
