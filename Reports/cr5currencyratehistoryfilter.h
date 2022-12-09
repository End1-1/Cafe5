#ifndef CR5CURRENCYRATEHISTORYFILTER_H
#define CR5CURRENCYRATEHISTORYFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CurrencyRateHistoryFilter;
}

class CR5CurrencyRateHistoryFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CurrencyRateHistoryFilter(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CR5CurrencyRateHistoryFilter();
    virtual QString condition() override;

private:
    Ui::CR5CurrencyRateHistoryFilter *ui;
};

#endif // CR5CURRENCYRATEHISTORYFILTER_H
