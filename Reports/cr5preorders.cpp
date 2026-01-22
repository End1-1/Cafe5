#include "cr5preorders.h"
#include "c5tablemodel.h"

CR5Preorders::CR5Preorders(QWidget *parent) :
    C5ReportWidget( parent)
{
    fLabel = tr("Preorders history");
    fIconName = ":/customers.png";
    fSimpleQuery = true;
    fSqlQuery = "select o.f_id, os.f_name as f_statename, o.f_datecreate, o.f_timecreate, o.f_datefor, o.f_timefor, "
                 "c.f_contact, c.f_phone, o.f_total, o.f_advance "
                 "from op_header o "
                 "left join c_partners c on c.f_id=o.f_customer "
                "inner join op_header_state os on os.f_id=o.f_state "
                 "order by os.f_id, o.f_datefor";
    fTranslation["f_id"] = tr("UUID");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_datecreate"] = tr("Date created");
    fTranslation["f_timecreate"] = tr("Time created");
    fTranslation["f_datefor"] = tr("Date for");
    fTranslation["f_timefor"] = tr("Time for");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_phone"] = tr("Phone");
    fTranslation["f_total"] = tr("Total");
    fTranslation["f_advance"] = tr("Advance");
    fColumnsSum.append("f_total");
    fColumnsSum.append("f_advance");
}

QToolBar *CR5Preorders::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5Preorders::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    if (fColumnsVisible["f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}
