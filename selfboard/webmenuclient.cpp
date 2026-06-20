#include "webmenuclient.h"

#include "menucache.h"

WebMenuClient::WebMenuClient()
{
}

QVector<MenuGroup> WebMenuClient::groups() const
{
    return MenuCache::instance().groups();
}

QVector<MenuDish> WebMenuClient::dishesByGroup(int groupId) const
{
    return MenuCache::instance().dishesByGroup(groupId);
}

QVector<MenuDish> WebMenuClient::popularDishes() const
{
    return MenuCache::instance().popularDishes();
}

MenuDish WebMenuClient::dishById(int dishId) const
{
    return MenuCache::instance().dishById(dishId);
}
