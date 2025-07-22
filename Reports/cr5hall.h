#ifndef CR5HALL_H
#define CR5HALL_H

#include "c5reportwidget.h"

class CR5Hall : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Hall(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5HALL_H
