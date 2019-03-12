#ifndef CR5DISHPRICESELFCOST_H
#define CR5DISHPRICESELFCOST_H

#include "c5reportwidget.h"

class CR5DishPriceSelfCostFilter;

class CR5DishPriceSelfCost : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPriceSelfCost(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

private:
    void buildQueryV1();

    void buildQueryV2();

    CR5DishPriceSelfCostFilter *fFilter;

private slots:
    void tblDoubleClickEvent(int row, int column, const QList<QVariant> &values);
};

#endif // CR5DISHPRICESELFCOST_H
