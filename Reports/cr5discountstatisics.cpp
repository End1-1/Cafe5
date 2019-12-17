#include "cr5discountstatisics.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5tablemodel.h"
#include "cr5discountstatisticsfilter.h"

CR5DiscountStatisics::CR5DiscountStatisics(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/discount.png";
    fLabel = tr("Discount statistics");
    fSimpleQuery = false;

    fMainTable = "b_history bh";
    fLeftJoinTables << "left join b_cards_discount bd on bd.f_id=bh.f_card [bd]"
                    << "left join b_card_types dt on dt.f_id=bd.f_mode [dt]"
                    << "left join o_header oh on oh.f_id=bh.f_order [oh]"
                    << "left join b_clients bc on bc.f_id=bd.f_client [bc]"
                       ;

    fColumnsFields << "oh.f_id"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_datecash"
                   << "dt.f_name as dtname"
                   << "bc.f_firstname"
                   << "bc.f_lastname"
                   << "bd.f_value"
                   << "sum(bh.f_data) as f_data"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                      ;

    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "dt.f_name as dtname"
                   << "bd.f_name"
                   << "bc.f_firstname"
                   << "bc.f_lastname"
                   << "bd.f_value"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_data"
                      ;

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_firstname"] = tr("First name");
    fTranslation["f_lastname"] = tr("Last name");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_value"] = tr("Value");
    fTranslation["dtname"] = tr("Mode");
    fTranslation["f_data"] = tr("Discount");
    fTranslation["f_amounttotal"] = tr("Total");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["dt.f_name as dtname"] = true;
    fColumnsVisible["bc.f_firstname"] = true;
    fColumnsVisible["bc.f_lastname"] = true;
    fColumnsVisible["bd.f_value"] = true;
    fColumnsVisible["sum(bh.f_data) as f_data"] = true;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;

    connect(this, SIGNAL(tblDoubleClicked(int, int, QList<QVariant>)), this, SLOT(openOrder(int, int, QList<QVariant>)));

    restoreColumnsVisibility();

    fFilterWidget = new CR5DiscountStatisticsFilter(dbParams);
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

void CR5DiscountStatisics::openOrder(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return;
    }
    if (values.count() == 0) {
        return;
    }
    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
    wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
}
