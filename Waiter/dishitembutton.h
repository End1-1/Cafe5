#ifndef DISHITEMBUTTON_H
#define DISHITEMBUTTON_H

#include <QPushButton>

class DishItemButton : public QPushButton
{
public:
    DishItemButton(QWidget *parent);

protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // DISHITEMBUTTON_H
