#ifndef CR5CONSUMPTIONBYSALESFILTER_H
#define CR5CONSUMPTIONBYSALESFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5ConsumptionBySalesFilter;
}

class CR5ConsumptionBySalesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5ConsumptionBySalesFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5ConsumptionBySalesFilter();

    virtual QString condition();

    int store();

    bool salesCounted();

    QDate date1();

    QDate date2();

private:
    Ui::CR5ConsumptionBySalesFilter *ui;
};

#endif // CR5CONSUMPTIONBYSALESFILTER_H
