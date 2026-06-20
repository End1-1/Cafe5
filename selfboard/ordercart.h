#pragma once

#include "menutypes.h"

#include <QHash>
#include <QVector>

struct CartLine
{
    QString key;
    int id = 0;
    MenuDish dish;
    int quantity = 0;
};

class OrderCart
{
public:
    void clear();
    void addDish(const MenuDish &dish, int qty = 1);
    void setQuantity(const QString &lineKey, int quantity);

    int itemCount() const;
    double totalAmount() const;
    QVector<CartLine> lines() const;
    bool isEmpty() const;

    QJsonObject dynamicAttributesJson(const MenuDish &dish) const;

private:
    QHash<QString, MenuDish> m_dishes;
    QHash<QString, int> m_qty;
};
