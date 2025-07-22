#ifndef CR5MFGENERALREPORT_H
#define CR5MFGENERALREPORT_H

#include "c5reportwidget.h"

class CR5MFGeneralReport : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5MFGeneralReport(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected slots:
    virtual void completeRefresh() override;
};

#endif // CR5MFGENERALREPORT_H
