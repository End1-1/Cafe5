#include "cr5costumerdebts.h"
#include "cr5costumerdebtsfilter.h"
#include "c5tablemodel.h"
#include "c5costumerdebtpayment.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"

CR5CostumerDebts::CR5CostumerDebts(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Customers debts");
    fSimpleQuery = false;

    fMainTable = "b_clients_debts cd";
    fLeftJoinTables << "left join c_partners c on c.f_id=cd.f_costumer [c]"
                    << "left join o_header oh on oh.f_id=cd.f_order [oh]"
                    << "left join b_car bc on bc.f_id=bco.f_car [bc]"
                    << "left join b_car_orders bco on bco.f_order=oh.f_id [bco]"
                       ;

    fColumnsFields << "cd.f_order"
                   << "cd.f_id"
                   << "c.f_contact"
                   << "cd.f_date"
                   << "concat(oh.f_prefix, f_hallid) as f_ordernum"
                   << "cd.f_govnumber"
                   << "sum(cd.f_amount) as f_amount"
                      ;

    fColumnsGroup << "cd.f_order"
                  << "cd.f_id"
                  << "c.f_contact"
                  << "cd.f_date"
                  << "concat(oh.f_prefix, f_hallid) as f_ordernum"
                  << "cd.f_govnumber"
                      ;

    fColumnsSum << "f_amount"
                      ;

    fTranslation["f_order"] = tr("UUID");
    fTranslation["f_id"] = tr("Row");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_ordernum"] = tr("Order");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["f_govnumber"] = tr("Gov. number");

    fColumnsVisible["cd.f_order"] = true;
    fColumnsVisible["cd.f_id"] = true;
    fColumnsVisible["c.f_contact"] = true;
    fColumnsVisible["cd.f_date"] = true;
    fColumnsVisible["concat(oh.f_prefix, f_hallid) as f_ordernum"] = true;
    fColumnsVisible["sum(cd.f_amount) as f_amount"] = true;
    fColumnsVisible["cd.f_govnumber"] = true;

    fHavindCondition = " having sum(cd.f_amount) <> 0 ";

    restoreColumnsVisibility();

    fFilterWidget = new CR5CostumerDebtsFilter(fDBParams);
    fFilter = static_cast<CR5CostumerDebtsFilter*>(fFilterWidget);
}

QToolBar *CR5CostumerDebts::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

int CR5CostumerDebts::newRow()
{
    C5CostumerDebtPayment *d = new C5CostumerDebtPayment(fDBParams);
    d->exec();
    delete d;
    return 0;
}

void CR5CostumerDebts::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["cd.f_order"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_order"], 0);
    }
    if (fColumnsVisible["cd.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5CostumerDebts::tblDoubleClicked(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (vals.empty()) {
        return true;
    }
    if (!fColumnsVisible["cd.f_order"] || !fColumnsVisible["cd.f_id"]) {
        C5Message::error(tr("Column UUID and Row must be included in the report"));
        return true;
    }
    if (!vals.at(0).toString().isEmpty()) {
        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
        wo->setOrder(vals.at(0).toString());
        return true;
    }
    if (!vals.at(1).toString().isEmpty()) {
        C5CostumerDebtPayment *d = new C5CostumerDebtPayment(fDBParams);
        d->setId(vals.at(1).toInt());
        d->exec();
        delete d;
    }
    return true;
}
