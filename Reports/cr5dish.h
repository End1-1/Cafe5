#ifndef CR5DISH_H
#define CR5DISH_H

#include "c5reportwidget.h"

class C5DishWidget;

class CR5Dish : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Dish(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

protected:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual void saveDataChanges();

private:
    C5DishWidget *fDishWidget;
};

#endif // CR5DISH_H
