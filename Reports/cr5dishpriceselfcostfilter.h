#ifndef CR5DISHPRICESELFCOSTFILTER_H
#define CR5DISHPRICESELFCOSTFILTER_H

#include "c5filterwidget.h"

namespace Ui
{
class CR5DishPriceSelfCostFilter;
}

class CR5DishPriceSelfCostFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DishPriceSelfCostFilter(QWidget *parent = nullptr);

    ~CR5DishPriceSelfCostFilter();

    virtual QString condition();

    virtual void saveFilter(QWidget *parent);

    virtual void restoreFilter(QWidget *parent);

    int viewMode();

    QString menuId();

    bool baseOnSale();

    const QDate d1();

    const QDate d2();

    int menuState();

    QString goods();

private:
    Ui::CR5DishPriceSelfCostFilter *ui;
};

#endif // CR5DISHPRICESELFCOSTFILTER_H
