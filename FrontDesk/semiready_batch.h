#pragma once

#include <QString>
#include <QVector>

struct SemireadyComponent
{
    int goodsId = 0;
    QString goodsName;
    double qty = 0;
};

struct SemireadyBatch
{
    QString id;
    int dishId = 0;
    QString dishName;
    double dishQty = 0;
    QVector<SemireadyComponent> components;
};
