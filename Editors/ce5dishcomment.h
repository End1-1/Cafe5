#ifndef CE5DISHCOMMENT_H
#define CE5DISHCOMMENT_H

#include "ce5editor.h"

namespace Ui {
class CE5DishComment;
}

class CE5DishComment : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishComment(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5DishComment();

    virtual QString title() {return tr("Dish comment");}

    virtual QString table() {return "d_dish_comment";}

private:
    Ui::CE5DishComment *ui;
};

#endif // CE5DISHCOMMENT_H
