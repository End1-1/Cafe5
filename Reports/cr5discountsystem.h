#ifndef CR5DISCOUNTSYSTEM_H
#define CR5DISCOUNTSYSTEM_H

#include "c5reportwidget.h"

class CR5DiscountSystem : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5DiscountSystem(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5DISCOUNTSYSTEM_H
