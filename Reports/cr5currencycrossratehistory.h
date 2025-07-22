#ifndef CR5CURRENCYCROSSRATEHISTORY_H
#define CR5CURRENCYCROSSRATEHISTORY_H

#include "c5reportwidget.h"

class CR5CurrencyCrossRateHistory : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5CurrencyCrossRateHistory(QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;
};

#endif // CR5CURRENCYCROSSRATEHISTORY_H
