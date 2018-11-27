#ifndef CE5DISHPART1_H
#define CE5DISHPART1_H

#include "ce5editor.h"

namespace Ui {
class CE5DishPart1;
}

class CE5DishPart1 : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishPart1(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5DishPart1();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5DishPart1 *ui;
};

#endif // CE5DISHPART1_H
