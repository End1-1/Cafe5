#ifndef CR5CURRENCYCROSSRATE_H
#define CR5CURRENCYCROSSRATE_H

#include "c5reportwidget.h"

class CR5CurrencyCrossRate : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5CurrencyCrossRate(QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;
};

#endif // CR5CURRENCYCROSSRATE_H
