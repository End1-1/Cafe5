#ifndef CR5CURRENCYRATEHISTORY_H
#define CR5CURRENCYRATEHISTORY_H

#include "c5reportwidget.h"

class CR5CurrencyRateHistory : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5CurrencyRateHistory(const QStringList &dbParams, QWidget *parent = nullptr);
    virtual QToolBar *toolBar();
};

#endif // CR5CURRENCYRATEHISTORY_H
