#ifndef CR5MFACTIVETASKS_H
#define CR5MFACTIVETASKS_H

#include "c5reportwidget.h"

class CR5MFActiveTasks : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5MFActiveTasks(QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

private slots:
    void configTablet();
};

#endif // CR5MFACTIVETASKS_H
