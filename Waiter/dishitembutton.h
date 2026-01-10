#pragma once

#include <QPushButton>

class DishItemButton : public QPushButton
{
public:
    DishItemButton(QWidget *parent);

protected:
    virtual void paintEvent(QPaintEvent *e);
};
