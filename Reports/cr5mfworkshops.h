#ifndef CR5MFWORKSHOPS_H
#define CR5MFWORKSHOPS_H

#include "c5reportwidget.h"

class CR5MFWorkshops : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5MFWorkshops(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5MFWORKSHOPS_H
