#ifndef CR5SALARYBYWORKERS_H
#define CR5SALARYBYWORKERS_H

#include "c5reportwidget.h"

class CR5SalaryByWorkers : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SalaryByWorkers(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual void restoreColumnsVisibility();

    virtual QToolBar *toolBar();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &vals);

};

#endif // CR5SALARYBYWORKERS_H
