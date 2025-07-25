#ifndef CR5SALARYBYWORKERS_H
#define CR5SALARYBYWORKERS_H

#include "c5reportwidget.h"

class CR5SalaryByWorkersFilter;

class CR5SalaryByWorkers : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5SalaryByWorkers(QWidget *parent = nullptr);

    virtual void restoreColumnsVisibility();

    virtual QToolBar *toolBar();

    virtual void buildQuery() override;

protected:
    virtual bool tblDoubleClicked(int row, int column, const QVector<QJsonValue> &vals);

private:
    QString fQuery;

    CR5SalaryByWorkersFilter *fFilter;

};

#endif // CR5SALARYBYWORKERS_H
