#ifndef CR5PREORDERS_H
#define CR5PREORDERS_H

#include "c5reportwidget.h"

class CR5Preorders : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5Preorders(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths() override;
};

#endif // CR5PREORDERS_H
