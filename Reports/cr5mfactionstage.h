#ifndef CR5MFACTIONSTAGE_H
#define CR5MFACTIONSTAGE_H

#include "c5reportwidget.h"

class CR5MFActionStage : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5MFActionStage(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5MFACTIONSTAGE_H
