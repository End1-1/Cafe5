#ifndef CR5SALEREMOVEDDISHESFILTER_H
#define CR5SALEREMOVEDDISHESFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SaleRemovedDishesFilter;
}

class CR5SaleRemovedDishesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SaleRemovedDishesFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5SaleRemovedDishesFilter();

    virtual QString condition();

    virtual QString filterText() override;

private:
    Ui::CR5SaleRemovedDishesFilter *ui;
};

#endif // CR5SALEREMOVEDDISHESFILTER_H
