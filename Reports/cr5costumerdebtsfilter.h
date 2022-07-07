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

    bool isTotal();

private:
    Ui::CR5CostumerDebtsFilter *ui;
};

#endif // CR5COSTUMERDEBTSFILTER_H
