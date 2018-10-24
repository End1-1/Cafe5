#ifndef CR5DISH_H
#define CR5DISH_H

#include "c5reportwidget.h"

class CR5Dish : public C5ReportWidget
{
public:
    CR5Dish(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5DISH_H
