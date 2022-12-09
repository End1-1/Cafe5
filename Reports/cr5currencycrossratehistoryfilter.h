#ifndef CR5CURRENCYCROSSRATEHISTORYFILTER_H
#define CR5CURRENCYCROSSRATEHISTORYFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CurrencyCrossRateHistoryFilter;
}

class CR5CurrencyCrossRateHistoryFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CurrencyCrossRateHistoryFilter(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CR5CurrencyCrossRateHistoryFilter();
    virtual QString condition() override;

private:
    Ui::CR5CurrencyCrossRateHistoryFilter *ui;
};

#endif // CR5CURRENCYCROSSRATEHISTORYFILTER_H
