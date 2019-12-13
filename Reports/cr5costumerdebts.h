#ifndef CR5COSTUMERDEBTS_H
#define CR5COSTUMERDEBTS_H

#include "c5reportwidget.h"

class CR5CostumerDebtsFilter;

class CR5CostumerDebts : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CostumerDebts(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

    virtual int newRow() override;

protected:
    virtual void restoreColumnsWidths() override;

private:
    CR5CostumerDebtsFilter *fFilter;

private slots:
    void openOrder(int row, int column, const QList<QVariant> &vals);
};

#endif // CR5COSTUMERDEBTS_H
