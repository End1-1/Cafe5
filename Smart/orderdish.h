#ifndef ORDERDISH_H
#define ORDERDISH_H

#include "dish.h"
#include <QWidget>

namespace Ui {
class OrderDish;
}


class OrderDish : public QWidget
{
    Q_OBJECT

public:
    explicit OrderDish(const Dish &d, QWidget *parent = nullptr);
    ~OrderDish();
    Dish fDish;
    void updateInfo();
    void setSelected(bool s);

private:
    Ui::OrderDish *ui;
};

#endif // ORDERDISH_H
