#ifndef CR5DISHPRICESELFCOST_H
#define CR5DISHPRICESELFCOST_H

#include "c5reportwidget.h"

class CR5DishPriceSelfCostFilter;

class CR5DishPriceSelfCost : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPriceSelfCost(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void buildQuery() override;

protected:
    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

private:
    void buildQueryV1();

    void buildQueryV2();

    void buildQueryV3();

    CR5DishPriceSelfCostFilter *fFilter;

};

#endif // CR5DISHPRICESELFCOST_H
