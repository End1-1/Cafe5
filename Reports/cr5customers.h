#ifndef CR5CUSTOMERS_H
#define CR5CUSTOMERS_H

#include "c5reportwidget.h"

class CR5Customers : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5Customers(const QStringList &dbParams, QWidget *parent = nullptr);
    virtual QToolBar *toolBar() override;
};

#endif // CR5CUSTOMERS_H
