#ifndef WEBMENUCLIENT_H
#define WEBMENUCLIENT_H

#include "imenuclient.h"

// Reads menu from MenuCache (HTTP preload at startup).
class WebMenuClient : public IMenuClient
{
public:
    WebMenuClient();

    QVector<MenuGroup> groups() const override;
    QVector<MenuDish> dishesByGroup(int groupId) const override;
    QVector<MenuDish> popularDishes() const override;
    MenuDish dishById(int dishId) const override;
};

#endif // WEBMENUCLIENT_H
