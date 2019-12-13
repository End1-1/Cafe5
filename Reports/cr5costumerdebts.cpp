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
    fLabel = tr("Costumers debts");
    fSimpleQuery = false;

    fMainTable = "b_clients_debts cd";
    fLeftJoinTables << "left join b_clients c on c.f_id=cd.f_costumer [c]"
                    << "left join o_header oh on oh.f_id=cd.f_order [oh]"
                       ;

    fColumnsFields << "cd.f_order"
                   << "cd.f_id"
                   << "concat(c.f_lastname, ' ', c.f_firstname) as f_costumername"
                   << "cd.f_date"
                   << "concat(oh.f_prefix, f_hallid) as f_ordernum"
                   << "sum(cd.f_amount) as f_amount"
                      ;

    fColumnsGroup << "cd.f_order"
                  << "cd.f_id"
                  << "concat(c.f_lastname, ' ', c.f_firstname) as f_costumername"
                  << "cd.f_date"
                  << "concat(oh.f_prefix, f_hallid) as f_ordernum"
                      ;

    fColumnsSum << "f_amount"
                      ;

    fTranslation["f_order"] = tr("UUID");
    fTranslation["f_id"] = tr("Row");
    fTranslation["f_costumername"] = tr("Costumer");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_ordernum"] = tr("Order");
    fTranslation["f_amount"] = tr("Amount");

    fColumnsVisible["cd.f_order"] = true;
    fColumnsVisible["cd.f_id"] = true;
    fColumnsVisible["concat(c.f_lastname, ' ', c.f_firstname) as f_costumername"] = true;
    fColumnsVisible["cd.f_date"] = true;
    fColumnsVisible["concat(oh.f_prefix, f_hallid) as f_ordernum"] = true;
    fColumnsVisible["sum(cd.f_amount) as f_amount"] = true;

    connect(this, SIGNAL(tblDoubleClicked(int, int, QList<QVariant>)), this, SLOT(openOrder(int, int, QList<QVariant>)));

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

void CR5CostumerDebts::openOrder(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (vals.empty()) {
        return;
    }
    if (!fColumnsVisible["cd.f_order"] || !fColumnsVisible["cd.f_id"]) {
        C5Message::error(tr("Column UUID and Row must be included in the report"));
        return;
    }
    if (!vals.at(0).toString().isEmpty()) {
        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
        wo->setOrder(vals.at(0).toString());
        return;
    }
    if (!vals.at(1).toString().isEmpty()) {
        C5CostumerDebtPayment *d = new C5CostumerDebtPayment(fDBParams);
        d->setId(vals.at(1).toInt());
        d->exec();
        delete d;
    }
}
