#ifndef CR5CASHDETAILEDFILTER_H
#define CR5CASHDETAILEDFILTER_H

#include "c5filterwidget.h"

namespace Ui
{
class CR5CashDetailedFilter;
}

class CR5CashDetailedFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CashDetailedFilter(QWidget *parent = nullptr);

    ~CR5CashDetailedFilter() override;

    virtual QString condition() override;

    virtual QString filterText() override;

    QDate date1() const;

    QDate date2() const;

    int cash();

    int shift();

private:
    Ui::CR5CashDetailedFilter* ui;
};

#endif // CR5CASHDETAILEDFILTER_H
