#ifndef DISH_H
#define DISH_H

#include <QString>
#include <QMetaType>

struct Dish {
    int id;
    int typeId;
    QString name;
    QString printer;
    QString adgCode;
    QString barcode;
    int store;
    double price;
    double qty;
    double netWeight;
    double cost;
    int color;
    int package;
    int quick;
    QString packageName;
    Dish() {
        id = 0;
        typeId = 0;
        store = 0;
        price = 0;
        qty = 1;
        color = -1;
        package = 0;
        quick = 0;
    }
};
Q_DECLARE_METATYPE(Dish*)
Q_DECLARE_METATYPE(Dish)

#endif // DISH_H
