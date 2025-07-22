#ifndef CR5DISHCOMMENT_H
#define CR5DISHCOMMENT_H

#include "c5reportwidget.h"

class CR5DishComment : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishComment(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5DISHCOMMENT_H
