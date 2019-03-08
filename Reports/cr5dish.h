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

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

protected:
    virtual void buildQuery();

    virtual void refreshData();

private:
    C5DishWidget *fDishWidget;

private:
    void setColors();

private slots:
    void translator();
};

#endif // CR5DISH_H
