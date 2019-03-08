#ifndef CR5DISHPRICESELFCOST_H
#define CR5DISHPRICESELFCOST_H

#include "c5reportwidget.h"

class CR5DishPriceSelfCost : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPriceSelfCost(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

    virtual void buildQuery();
};

#endif // CR5DISHPRICESELFCOST_H
