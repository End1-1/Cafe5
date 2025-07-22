#ifndef CR5DISHPACKAGE_H
#define CR5DISHPACKAGE_H

#include "c5reportwidget.h"

class CR5DishPackage : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5DishPackage(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    void editDishList();
};

#endif // CR5DISHPACKAGE_H
