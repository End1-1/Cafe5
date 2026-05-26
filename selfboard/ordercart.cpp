#include "ordercart.h"

void OrderCart::clear()
{
    m_dishes.clear();
    m_qty.clear();
}

void OrderCart::addDish(const MenuDish &dish)
{
    if (dish.id <= 0) {
        return;
    }
    m_dishes.insert(dish.id, dish);
    m_qty[dish.id] = m_qty.value(dish.id, 0) + 1;
}

int OrderCart::itemCount() const
{
    int n = 0;
    for (int q : m_qty) {
        n += q;
    }
    return n;
}

double OrderCart::totalAmount() const
{
    double sum = 0;
    for (auto it = m_qty.constBegin(); it != m_qty.constEnd(); ++it) {
        const MenuDish dish = m_dishes.value(it.key());
        sum += dish.price * it.value();
    }
    return sum;
}
