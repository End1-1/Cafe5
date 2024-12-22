#ifndef CR5DISHPRICESELFCOST_H
#define CR5DISHPRICESELFCOST_H

#include "c5reportwidget.h"

class CR5DishPriceSelfCostFilter;

class CR5DishPriceSelfCost : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPriceSelfCost(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual void buildQuery();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values) override;

private:
    void buildQueryV1();

    void buildQueryV2();

    void buildQueryV3();

    CR5DishPriceSelfCostFilter *fFilter;

};

#endif // CR5DISHPRICESELFCOST_H
