#include "cr5mfgeneralreport.h"
#include "cr5mfgeneralreportfilter.h"

CR5MFGeneralReport::CR5MFGeneralReport(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Manufacture general report");
    fFilterWidget = new CR5MFGeneralReportFilter(dbParams);

    fSimpleQuery = false;
    fMainTable = "mf_daily_process p";

    fLeftJoinTables << "left join mf_actions_group pr on pr.f_id=p.f_product [pr]"
                    << "left join mf_actions ac on ac.f_id=p.f_process [ac]"
                    << "left join s_user w on w.f_id=p.f_worker [w]"
                    << "left join s_user_group wg on wg.f_id=w.f_group [wg]";

    fColumnsFields << "p.f_date"
                    << "wg.f_name as f_positionname"
                    << "concat(w.f_last, ' ', w.f_first) as f_workername"
                    << "pr.f_name as f_productname"
                    << "ac.f_name as f_processname"
                    << "sum(p.f_qty) as f_qty"
                    << "p.f_price"
                    << "sum(p.f_qty*p.f_price) as f_total";

    fColumnsGroup << "p.f_date"
                    << "wg.f_name as f_positionname"
                    << "concat(w.f_last, ' ', w.f_first) as f_workername"
                    << "pr.f_name as f_productname"
                    << "ac.f_name as f_processname"
                    << "p.f_qty"
                    << "p.f_price";

    fColumnsSum << "f_qty"
                << "f_total";

    fColumnsOrder << "p.f_date"
                  << "concat(w.f_last, ' ', w.f_first)";


    fTranslation["f_date"] = tr("Date");
    fTranslation["f_positionname"] = tr("Position");
    fTranslation["f_workername"] = tr("Worker");
    fTranslation["f_productname"] = tr("Product");
    fTranslation["f_processname"] = tr("Process");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Total");

    fColumnsVisible["p.f_date"] = true;
    fColumnsVisible["wg.f_name as f_positionname"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_workername"] = true;
    fColumnsVisible["pr.f_name as f_productname"] = true;
    fColumnsVisible["ac.f_name as f_processname"] = true;
    fColumnsVisible["sum(p.f_qty) as f_qty"] = true;
    fColumnsVisible["p.f_price"] = false;
    fColumnsVisible["sum(p.f_qty*p.f_price) as f_total"] = true;
}

QToolBar *CR5MFGeneralReport::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}
