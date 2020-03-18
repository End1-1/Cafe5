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
    fSimpleQuery = false;

    fMainTable = "s_salary_body sb";
    fLeftJoinTables << "inner join a_header h on h.f_id=sb.f_header [h]"
                    << "inner join s_salary_options so on so.f_id=h.f_id [so]"
                    << "left join s_salary_shift sh on sh.f_id=so.f_shift [sh]"
                    << "inner join s_user u on u.f_id=sb.f_user [u]"
                       ;

    fColumnsFields << "h.f_id as f_header"
                   << "h.f_date"
                   << "sh.f_name"
                   << "concat(u.f_last, ' ', u.f_first) as f_employee"
                   << "sum(sb.f_amount) as f_total"
                      ;

    fColumnsGroup << "h.f_id as f_header"
                  << "h.f_date"
                  << "sh.f_name"
                  << "concat(u.f_last, ' ', u.f_first) as f_employee"
                      ;

    fColumnsSum << "f_total"
                      ;

    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_name"] = tr("Shift");
    fTranslation["f_employee"] = tr("Employee");
    fTranslation["f_total"] = tr("Total");

    fColumnsVisible["h.f_id as f_header"] = true;
    fColumnsVisible["h.f_date"] = true;
    fColumnsVisible["sh.f_name"] = true;
    fColumnsVisible["concat(u.f_last, ' ', u.f_first) as f_employee"] = true;
    fColumnsVisible["sum(sb.f_amount) as f_total"] = true;

    restoreColumnsVisibility();
    fFilterWidget = new CR5SalaryByWorkersFilter(fDBParams);

}

void CR5SalaryByWorkers::restoreColumnsVisibility()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["h.f_id as f_header"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_header"], 0);
    }
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

bool CR5SalaryByWorkers::tblDoubleClicked(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["h.f_id as f_header"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (vals.isEmpty()) {
        return true;
    }
    auto *sd = __mainWindow->createTab<C5SalaryDoc>(fDBParams);
    if (!sd->openDoc(vals.at(0).toString())) {
        __mainWindow->removeTab(sd);
    }
    return true;
}
