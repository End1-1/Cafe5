#ifndef CR5COSTUMERDEBTS_H
#define CR5COSTUMERDEBTS_H

#include "c5reportwidget.h"

class CR5CostumerDebtsFilter;

class CR5CostumerDebts : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CostumerDebts(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void buildQuery() override;

protected:
    virtual void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &vals) override;

private:
    CR5CostumerDebtsFilter *fFilter;

private slots:
    void newCustomerPayment();

    void newPartnerPayment();

    void queryDebt3();

};

#endif // CR5COSTUMERDEBTS_H
