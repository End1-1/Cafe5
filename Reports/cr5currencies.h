#ifndef CR5CURRENCIES_H
#define CR5CURRENCIES_H

#include "c5reportwidget.h"

class CR5Currencies : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Currencies(QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;
};

#endif // CR5CURRENCIES_H
