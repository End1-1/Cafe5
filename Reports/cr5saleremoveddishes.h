#ifndef CR5SALEREMOVEDDISHES_H
#define CR5SALEREMOVEDDISHES_H

#include "c5reportwidget.h"

class CR5SaleRemovedDishesFilter;

class CR5SaleRemovedDishes : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SaleRemovedDishes(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &v);

private:
    CR5SaleRemovedDishesFilter *fFilter;

};

#endif // CR5SALEREMOVEDDISHES_H
