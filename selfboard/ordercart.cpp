#include "ordercart.h"

#include "menuhelpers.h"

#include <QJsonObject>

void OrderCart::clear()
{
    m_dishes.clear();
    m_qty.clear();
}

void OrderCart::addDish(const MenuDish &dish, int qty)
{
    if (dish.id <= 0 || qty <= 0) {
        return;
    }

    const QString key = MenuHelpers::cartLineKey(dish);
    m_dishes.insert(key, dish);
    m_qty[key] = m_qty.value(key, 0) + qty;
}

void OrderCart::setQuantity(const QString &lineKey, int quantity)
{
    if (lineKey.isEmpty() || !m_qty.contains(lineKey)) {
        return;
    }
    if (quantity <= 0) {
        m_qty.remove(lineKey);
        m_dishes.remove(lineKey);
        return;
    }
    m_qty[lineKey] = quantity;
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

QVector<CartLine> OrderCart::lines() const
{
    QVector<CartLine> result;
    result.reserve(m_qty.size());
    for (auto it = m_qty.constBegin(); it != m_qty.constEnd(); ++it) {
        CartLine line;
        line.key = it.key();
        line.dish = m_dishes.value(it.key());
        line.id = line.dish.id;
        line.quantity = it.value();
        result.append(line);
    }
    return result;
}

bool OrderCart::isEmpty() const
{
    return m_qty.isEmpty();
}

QJsonObject OrderCart::dynamicAttributesJson(const MenuDish &dish) const
{
    return dish.dynamicAttributes().toJsonObject();
}
