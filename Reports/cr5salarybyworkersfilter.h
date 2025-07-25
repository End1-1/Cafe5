#ifndef CR5SALARYBYWORKERSFILTER_H
#define CR5SALARYBYWORKERSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SalaryByWorkersFilter;
}

class CR5SalaryByWorkersFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SalaryByWorkersFilter(QWidget *parent = nullptr);

    ~CR5SalaryByWorkersFilter();

    virtual QString condition();

    QString replaceFitler();

private:
    Ui::CR5SalaryByWorkersFilter *ui;
};

#endif // CR5SALARYBYWORKERSFILTER_H
