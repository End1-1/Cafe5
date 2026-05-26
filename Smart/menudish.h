#ifndef MENUDISH_H
#define MENUDISH_H

#include <QWidget>

namespace Ui
{
class menudish;
}

struct Dish;

class MenuDish : public QWidget
{
    Q_OBJECT

public:
    explicit MenuDish(Dish *d, bool groupVisible);
    ~MenuDish();
    Dish* fDish;

    void setImage();

private:
    Ui::menudish* ui;
};

#endif // MENUDISH_H
