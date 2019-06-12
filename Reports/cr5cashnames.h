#ifndef CR5CASHNAMES_H
#define CR5CASHNAMES_H

#include "c5reportwidget.h"

class CR5CashNames : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CashNames(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;
};

#endif // CR5CASHNAMES_H
