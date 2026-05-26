#ifndef WEBMENUCLIENT_H
#define WEBMENUCLIENT_H

#include "imenuclient.h"

// TODO: load groups/dishes/popular from SelfBoard web API route.
class WebMenuClient : public IMenuClient
{
public:
    explicit WebMenuClient(const QString &baseUrl);

    QVector<MenuGroup> groups() const override;
    QVector<MenuDish> dishesByGroup(int groupId) const override;
    QVector<MenuDish> popularDishes() const override;
    MenuDish dishById(int dishId) const override;

private:
    QString m_baseUrl;
};

#endif // WEBMENUCLIENT_H
