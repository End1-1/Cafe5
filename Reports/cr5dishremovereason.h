#ifndef CR5DISHREMOVEREASON_H
#define CR5DISHREMOVEREASON_H

#include "c5reportwidget.h"

class CR5DishRemoveReason : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishRemoveReason(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5DISHREMOVEREASON_H
