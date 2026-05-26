#ifndef IMENUCLIENT_H
#define IMENUCLIENT_H

#include "menutypes.h"

// Data source for menu screen. Implementations: MenuStubClient (dev), WebMenuClient (API).
class IMenuClient
{
public:
    virtual ~IMenuClient() = default;

    virtual QVector<MenuGroup> groups() const = 0;
    virtual QVector<MenuDish> dishesByGroup(int groupId) const = 0;
    virtual QVector<MenuDish> popularDishes() const = 0;
    virtual MenuDish dishById(int dishId) const = 0;
};

#endif // IMENUCLIENT_H
