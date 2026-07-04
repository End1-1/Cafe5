#pragma once

#include "menutypes.h"

#include <QHash>
#include <QSet>
#include <QVector>

struct CartLine
{
    QString key;
    int id = 0;
    MenuDish dish;
    int quantity = 0;
    bool editable = false;
};

class OrderCart
{
public:
    void clear();
    void addDish(const MenuDish &dish, int qty = 1, bool editable = false);
    void setQuantity(const QString &lineKey, int quantity);

    int itemCount() const;
    double totalAmount() const;
    QVector<CartLine> lines() const;
    bool isEmpty() const;
    bool isLineEditable(const QString &lineKey) const;

    QJsonObject dynamicAttributesJson(const MenuDish &dish) const;

private:
    QHash<QString, MenuDish> m_dishes;
    QHash<QString, int> m_qty;
    QSet<QString> m_editableLineKeys;
};
