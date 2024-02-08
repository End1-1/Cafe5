#ifndef CR5CONSUPTIONREASON_H
#define CR5CONSUPTIONREASON_H

#include "c5reportwidget.h"

class CR5ConsuptionReasonFilter;

class CR5ConsuptionReason : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5ConsuptionReason(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

    virtual void restoreColumnsWidths() override;

    void setFilterParams(const QDate &d1, const QDate &d2, int store, int goods);

    virtual void buildQuery() override;

private:
    CR5ConsuptionReasonFilter *fFilter;

};

#endif // CR5CONSUPTIONREASON_H
