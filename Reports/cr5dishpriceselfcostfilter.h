#ifndef CR5DISHPRICESELFCOSTFILTER_H
#define CR5DISHPRICESELFCOSTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5DishPriceSelfCostFilter;
}

class CR5DishPriceSelfCostFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DishPriceSelfCostFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5DishPriceSelfCostFilter();

    virtual QString condition();

    virtual void saveFilter(QWidget *parent);

    virtual void restoreFilter(QWidget *parent);

    bool isSimpleView();

    QString menuId();

private:
    Ui::CR5DishPriceSelfCostFilter *ui;
};

#endif // CR5DISHPRICESELFCOSTFILTER_H
