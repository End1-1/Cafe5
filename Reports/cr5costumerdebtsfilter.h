#ifndef CR5COSTUMERDEBTSFILTER_H
#define CR5COSTUMERDEBTSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CostumerDebtsFilter;
}

class CR5CostumerDebtsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CostumerDebtsFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5CostumerDebtsFilter();

    virtual QString condition() override;

    virtual QString filterText() override;

    QString sourceCond();

    QString flagCond();

    int viewMode();

    QString date1();

    QString date2();

    QString manager();

    QString flag();

    bool debtMode();

private:
    Ui::CR5CostumerDebtsFilter *ui;
};

#endif // CR5COSTUMERDEBTSFILTER_H
