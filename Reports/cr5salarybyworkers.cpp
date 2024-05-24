#include "cr5salarybyworkers.h"
#include "c5tablemodel.h"
#include "cr5salarybyworkersfilter.h"
#include "c5salarydoc.h"
#include "c5mainwindow.h"

CR5SalaryByWorkers::CR5SalaryByWorkers(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/employee.png";
    fLabel = tr("Salary by workers");
    fSimpleQuery = true;
    fQuery = "SELECT sp.f_id, sp.f_date AS `Ամսաթիվ`, g.f_name AS `Հաստիկ`, concat_ws(' ', u.f_last, u.f_first) AS `Աշխատող`, "
             "if (sp.f_cashdoc is NULL, sp.f_amount, sp.f_amount*-1) AS `Գումար`  "
             "FROM s_salary_payment sp "
             "LEFT JOIN s_user u ON u.f_id=sp.f_worker "
             "LEFT JOIN s_user_group g ON g.f_id=u.f_group "
             " %filter "
             "ORDER BY 1, 3";
    fColumnsSum.append("գումար");
    restoreColumnsVisibility();
    fFilterWidget = new CR5SalaryByWorkersFilter(fDBParams);
    fFilter = static_cast<CR5SalaryByWorkersFilter *>(fFilterWidget);
}

void CR5SalaryByWorkers::restoreColumnsVisibility()
{
    C5Grid::restoreColumnsWidths();
    fTableView->setColumnWidth(0, 0);
}

QToolBar *CR5SalaryByWorkers::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5SalaryByWorkers::buildQuery()
{
    fSqlQuery = fQuery;
    fSqlQuery.replace("%filter", fFilter->replaceFitler());
    C5ReportWidget::buildQuery();
}

bool CR5SalaryByWorkers::tblDoubleClicked(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(vals);
    return true;
}
