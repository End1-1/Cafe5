#ifndef CR5DISHPART1_H
#define CR5DISHPART1_H

#include "c5reportwidget.h"

class CR5DishPart1 : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPart1(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5DISHPART1_H
