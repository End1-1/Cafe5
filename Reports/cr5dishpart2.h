#ifndef CR5DISHPART2_H
#define CR5DISHPART2_H

#include "c5reportwidget.h"

class CR5DishPart2 : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPart2(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();
};

#endif // CR5DISHPART2_H
