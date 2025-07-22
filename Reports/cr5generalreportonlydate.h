#ifndef CR5GENERALREPORTONLYDATE_H
#define CR5GENERALREPORTONLYDATE_H

#include "c5reportwidget.h"

class CR5MFGeneralReportFilter;

class CR5GeneralReportOnlyDate : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5GeneralReportOnlyDate();

    virtual void buildQuery() override;

    virtual QToolBar *toolBar() override;

protected slots:
    virtual void completeRefresh() override;

private:
    CR5MFGeneralReportFilter *fFilter;
};

#endif // CR5GENERALREPORTONLYDATE_H
