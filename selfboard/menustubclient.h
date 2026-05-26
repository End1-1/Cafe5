#ifndef MENUSTUBCLIENT_H
#define MENUSTUBCLIENT_H

#include "imenuclient.h"

class MenuStubClient : public IMenuClient
{
public:
    MenuStubClient();

    QVector<MenuGroup> groups() const override;
    QVector<MenuDish> dishesByGroup(int groupId) const override;
    QVector<MenuDish> popularDishes() const override;
    MenuDish dishById(int dishId) const override;

private:
    QVector<MenuGroup> m_groups;
    QVector<MenuDish> m_dishes;
};

#endif // MENUSTUBCLIENT_H
