#include "cr5cashmovement.h"
#include "cr5cashmovementfilter.h"
#include "c5tablemodel.h"

CR5CashMovement::CR5CashMovement(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cash movement");

    fSimpleQuery = false;
    fMainTable = "e_cash ec";
    fLeftJoinTables << "left join a_header h on h.f_id=ec.f_header [h]"
                    << "left join e_cash_names cn on cn.f_id=ec.f_cash [cn]"
                    << "left join b_clients_debts cd on cd.f_cash=h.f_id [cd]"
                       ;

    fColumnsFields << "h.f_id"
                   << "cn.f_name as f_cashname"
                   << "h.f_date"
                   << "ec.f_remarks"
                   << "ec.f_amount*ec.f_sign as f_amount"
                    ;

    fColumnsSum << "f_amount"
                      ;

    fTranslation["f_id"] = tr("ID");
    fTranslation["f_cashname"] = tr("Cash");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_remarks"] = tr("Purpose");
    fTranslation["f_amount"] = tr("Amount");

    fColumnsVisible["h.f_id"] = true;
    fColumnsVisible["cn.f_name as f_cashname"] = true;
    fColumnsVisible["h.f_date"] = true;
    fColumnsVisible["ec.f_remarks"] = true;
    fColumnsVisible["ec.f_amount*ec.f_sign as f_amount"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5CashMovementFilter(fDBParams);
}

QToolBar *CR5CashMovement::toolBar()
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

void CR5CashMovement::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["h.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}
