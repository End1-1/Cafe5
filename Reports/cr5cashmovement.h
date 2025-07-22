#ifndef CR5CASHMOVEMENT_H
#define CR5CASHMOVEMENT_H

#include "c5reportwidget.h"

class CR5CashMovement : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CashMovement(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths() override;
};

#endif // CR5CASHMOVEMENT_H
