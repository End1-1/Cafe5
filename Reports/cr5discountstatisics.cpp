#include "cr5discountstatisics.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5tablemodel.h"
#include "cr5discountstatisticsfilter.h"

CR5DiscountStatisics::CR5DiscountStatisics(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/discount.png";
    fLabel = tr("Discount statistics");
    fSimpleQuery = false;

    fMainTable = "b_history bh";
    fLeftJoinTables << "left join b_cards_discount bd on bd.f_id=bh.f_card [bd]"
                    << "left join b_card_types dt on dt.f_id=bd.f_mode [dt]"
                    << "left join o_header oh on oh.f_id=bh.f_id [oh]"
                    << "left join c_partners bc on bc.f_id=bd.f_client [bc]"
                       ;

    fColumnsFields << "oh.f_id"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_datecash"
                   << "dt.f_name as dtname"
                   << "bd.f_code"
                   << "bc.f_contact"
                   << "bd.f_value"
                   << "sum(bh.f_data) as f_data"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                      ;

    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "dt.f_name as dtname"
                   << "bd.f_name"
                   << "bc.f_contact"
                   << "bd.f_value"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_data"
                      ;

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_contact"] = tr("Contact name");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_value"] = tr("Value");
    fTranslation["dtname"] = tr("Mode");
    fTranslation["f_code"] = tr("Code");
    fTranslation["f_data"] = tr("Discount");
    fTranslation["f_amounttotal"] = tr("Total");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["dt.f_name as dtname"] = true;
    fColumnsVisible["bd.f_code"] = true;
    fColumnsVisible["bc.f_contact"] = true;
    fColumnsVisible["bd.f_value"] = true;
    fColumnsVisible["sum(bh.f_data) as f_data"] = true;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5DiscountStatisticsFilter();
}

QToolBar *CR5DiscountStatisics::toolBar()
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

void CR5DiscountStatisics::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["oh.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5DiscountStatisics::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (values.count() == 0) {
        return true;
    }
    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
    wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
    return true;
}
