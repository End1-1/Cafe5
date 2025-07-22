#ifndef CE5DISHREMOVEREASON_H
#define CE5DISHREMOVEREASON_H

#include "ce5editor.h"

namespace Ui {
class CE5DishRemoveReason;
}

class CE5DishRemoveReason : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishRemoveReason(QWidget *parent = nullptr);

    ~CE5DishRemoveReason();

    virtual QString title() {return tr("Dish remove reason");}

    virtual QString table() {return "o_dish_remove_reason";}

private:
    Ui::CE5DishRemoveReason *ui;
};

#endif // CE5DISHREMOVEREASON_H
