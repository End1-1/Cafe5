#ifndef DISH_H
#define DISH_H

#include <QString>
#include <QMetaType>

struct Dish {
    int menuid;
    int id;
    int typeId;
    int state;
    QString typeName;
    QString name;
    QString printer;
    QString printer2;
    QString adgCode;
    QString barcode;
    QString modificator;
    int store;
    double price;
    double discount;
    double specialDiscount;
    double qty;
    double qty2;
    double netWeight;
    double cost;
    int color;
    int package;
    int quick;
    int qrRequired = 0;
    QString packageName;
    QString obodyId;
    QString f_emarks;
    Dish()
    {
        menuid = 0;
        id = 0;
        state = 1;
        typeId = 0;
        store = 0;
        price = 0;
        qty = 1;
        qty2 = 0;
        color = -1;
        package = 0;
        quick = 0;
        discount = 0;
        specialDiscount = 0;
    }
    Dish(Dish *d)
    {
        id = d->id;
        state = d->state;
        typeId = d->typeId;
        typeName = d->typeName;
        name = d->name;
        printer = d->printer;
        printer2 = d->printer2;
        adgCode = d->adgCode;
        barcode = d->barcode;
        modificator = d->modificator;
        store = d->store;
        price = d->price;
        discount = d->discount;
        specialDiscount = d->specialDiscount;
        qty = d->qty;
        qty2 = d->qty2;
        netWeight = d->netWeight;
        cost = d->cost;
        color = d->color;
        package = d->package;
        quick = d->quick;
        packageName = d->packageName;
        obodyId = d->obodyId;
        f_emarks = d->f_emarks;
    }
};
Q_DECLARE_METATYPE(Dish *)
Q_DECLARE_METATYPE(Dish)

#endif // DISH_H
