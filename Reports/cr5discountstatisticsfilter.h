#ifndef CR5DISCOUNTSTATISTICSFILTER_H
#define CR5DISCOUNTSTATISTICSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5DiscountStatisticsFilter;
}

class CR5DiscountStatisticsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DiscountStatisticsFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5DiscountStatisticsFilter();

    virtual QString condition() override;

private:
    Ui::CR5DiscountStatisticsFilter *ui;
};

#endif // CR5DISCOUNTSTATISTICSFILTER_H
