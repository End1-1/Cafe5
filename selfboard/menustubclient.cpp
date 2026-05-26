#include "menustubclient.h"

namespace {

MenuDish makeDish(int id, int groupId, const QString &groupName, const QString &name, double price,
                   const QString &imagePath = QStringLiteral(":/res/dish_placeholder.png"),
                   const QString &prepTime = QStringLiteral("10 - 15 Min"))
{
    MenuDish dish;
    dish.id = id;
    dish.groupId = groupId;
    dish.groupName = groupName;
    dish.name = name;
    dish.imagePath = imagePath;
    dish.price = price;
    dish.prepTime = prepTime;
    return dish;
}

} // namespace

MenuStubClient::MenuStubClient()
{
    m_groups = {
        {1, QStringLiteral("Burger"), QStringLiteral(":/res/cat_burger.png")},
        {2, QStringLiteral("BBQ"), QStringLiteral(":/res/cat_bbq.png")},
        {3, QStringLiteral("Coffee"), QStringLiteral(":/res/cat_coffee.png")},
        {4, QStringLiteral("Chips"), QStringLiteral(":/res/cat_chips.png")},
        {5, QStringLiteral("Spagetti"), QStringLiteral(":/res/cat_spaghetti.png")},
        {6, QStringLiteral("Gelato"), QStringLiteral(":/res/cat_gelato.png")},
        {7, QStringLiteral("Pizza"), QStringLiteral(":/res/cat_pizza.png")},
        {8, QStringLiteral("Hotdog"), QStringLiteral(":/res/cat_hotdog.png")},
    };

    int dishId = 1;
    auto addGroupDishes = [&](int groupId, const QString &groupName, const QStringList &names, double basePrice) {
        for (const QString &name : names) {
            m_dishes.append(makeDish(dishId++, groupId, groupName, name, basePrice));
            basePrice += 50.0;
        }
    };

    addGroupDishes(1, QStringLiteral("Burger"),
                   {QStringLiteral("Best Burger Ever"), QStringLiteral("Cheese Burger"),
                    QStringLiteral("Chicken Burger"), QStringLiteral("Double Burger"),
                    QStringLiteral("Bacon Burger"), QStringLiteral("Veggie Burger")},
                   1200.0);
    addGroupDishes(2, QStringLiteral("BBQ"),
                   {QStringLiteral("BBQ Ribs"), QStringLiteral("Grilled Chicken"),
                    QStringLiteral("Pork Steak"), QStringLiteral("Mixed Grill"),
                    QStringLiteral("Beef Skewers"), QStringLiteral("Lamb Chops"),
                    QStringLiteral("Smoked Wings"), QStringLiteral("Corn Ribs"), QStringLiteral("BBQ Platter")},
                   1800.0);
    addGroupDishes(3, QStringLiteral("Coffee"),
                   {QStringLiteral("Espresso"), QStringLiteral("Cappuccino"),
                    QStringLiteral("Latte"), QStringLiteral("Americano"),
                    QStringLiteral("Mocha"), QStringLiteral("Flat White"),
                    QStringLiteral("Hot Chocolate"), QStringLiteral("Iced Latte"), QStringLiteral("Macchiato")},
                   500.0);
    addGroupDishes(4, QStringLiteral("Chips"),
                   {QStringLiteral("Classic Fries"), QStringLiteral("Cheese Fries"),
                    QStringLiteral("Spicy Fries")},
                   600.0);
    addGroupDishes(5, QStringLiteral("Spagetti"),
                   {QStringLiteral("Carbonara"), QStringLiteral("Bolognese"),
                    QStringLiteral("Pesto Pasta")},
                   1100.0);
    addGroupDishes(6, QStringLiteral("Gelato"),
                   {QStringLiteral("Vanilla Gelato"), QStringLiteral("Chocolate Gelato"),
                    QStringLiteral("Strawberry Gelato")},
                   700.0);
    addGroupDishes(7, QStringLiteral("Pizza"),
                   {QStringLiteral("Margarita"), QStringLiteral("Pepperoni"),
                    QStringLiteral("Four Cheese"), QStringLiteral("Vegetarian Pizza")},
                   1300.0);
    addGroupDishes(8, QStringLiteral("Hotdog"),
                   {QStringLiteral("Classic Hotdog"), QStringLiteral("Cheese Hotdog"),
                    QStringLiteral("Spicy Hotdog")},
                   800.0);
}

QVector<MenuGroup> MenuStubClient::groups() const
{
    return m_groups;
}

QVector<MenuDish> MenuStubClient::dishesByGroup(int groupId) const
{
    QVector<MenuDish> result;
    for (const MenuDish &dish : m_dishes) {
        if (dish.groupId == groupId) {
            result.append(dish);
        }
    }
    return result;
}

QVector<MenuDish> MenuStubClient::popularDishes() const
{
    return {
        makeDish(9001, 7, QStringLiteral("Pizza"), QStringLiteral("Margarita"), 500.0),
        makeDish(9002, 7, QStringLiteral("Pizza"), QStringLiteral("Margarita"), 500.0),
        makeDish(9003, 7, QStringLiteral("Pizza"), QStringLiteral("Margarita"), 500.0),
    };
}

MenuDish MenuStubClient::dishById(int dishId) const
{
    for (const MenuDish &dish : m_dishes) {
        if (dish.id == dishId) {
            return dish;
        }
    }
    for (const MenuDish &dish : popularDishes()) {
        if (dish.id == dishId) {
            return dish;
        }
    }
    return {};
}
