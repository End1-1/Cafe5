#include "cr5salesbydishes.h"
#include "cr5salesbydishesfilter.h"
#include "c5waiterorder.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "ce5editor.h"
#include "c5dishwidget.h"

CR5SalesByDishes::CR5SalesByDishes(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/delete.png";
    fLabel = tr("Sales, dishes");
    fSimpleQuery = false;

    fMainTable = "o_body ob";
    fLeftJoinTables << "left join o_header oh on oh.f_id=ob.f_header [oh]"
                    << "left join h_halls h on h.f_id=oh.f_hall [h]"
                    << "left join h_tables t on t.f_id=oh.f_table [t]"
                    << "left join o_state os on os.f_id=oh.f_state [os]"
                    << "left join o_body_state bs on bs.f_id=ob.f_state [bs]"
                    << "left join d_dish d on d.f_id=ob.f_dish [d]"
                    << "left join d_part2 dp on dp.f_id=d.f_part [dp]"
                    << "left join d_part1 dpd on dpd.f_id=dp.f_part [dpd]"
                    << "left join c_storages cst on cst.f_id=ob.f_store [cst]"
                       ;

    fColumnsFields << "oh.f_id as f_header"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_order"
                   << "os.f_name as f_statename"
                   << "h.f_name as f_hallname"
                   << "t.f_name as f_tablename"
                   << "oh.f_datecash"
                   << "oh.f_timeclose"
                   << "bs.f_name as f_dishstate"
                   << "dpd.f_name as f_dishdept"
                   << "dp.f_name as f_dishpart"
                   << "d.f_id as f_dishid"
                   << "d.f_name as f_dishname"
                   << "cst.f_name as f_storename"
                   << "ob.f_removereason"
                   << "sum(ob.f_qty2) as f_qty"
                   << "sum(ob.f_total) as f_total"
                      ;

    fColumnsGroup << "oh.f_id as f_header"
                  << "concat(oh.f_prefix, oh.f_hallid) as f_order"
                  << "h.f_name as f_hallname"
                  << "t.f_name as f_tablename"
                  << "os.f_name as f_statename"
                  << "oh.f_datecash"
                  << "oh.f_timeclose"
                  << "dpd.f_name as f_dishdept"
                  << "dp.f_name as f_dishpart"
                  << "d.f_id as f_dishid"
                  << "d.f_name as f_dishname"
                  << "bs.f_name as f_dishstate"
                  << "ob.f_removereason"
                  <<"cst.f_name as f_storename"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                      ;

    fColumnsOrder << "oh.f_datecash"
                  << "oh.f_timeclose";

    fTranslation["f_header"] = tr("Header");
    fTranslation["f_order"] = tr("Order");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_tablename"] = tr("Table");
    fTranslation["f_statename"] = tr("Order state");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_timeclose"] = tr("Time, close");
    fTranslation["f_dishdept"] = tr("Dept");
    fTranslation["f_dishpart"] = tr("Type of dish");
    fTranslation["f_dishid"] = tr("Dish code");
    fTranslation["f_dishname"] = tr("Dish");
    fTranslation["f_dishstate"] = tr("Dish state");
    fTranslation["f_removereason"] = tr("Remove reason");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_total"] = tr("Total");

    fColumnsVisible["oh.f_id as f_header"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["oh.f_timeclose"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_order"] = true;
    fColumnsVisible["h.f_name as f_hallname"] = true;
    fColumnsVisible["t.f_name as f_tablename"] = true;
    fColumnsVisible["dpd.f_name as f_dishdept"] = true;
    fColumnsVisible["dp.f_name as f_dishpart"] = true;
    fColumnsVisible["d.f_id as f_dishid"] = false;
    fColumnsVisible["d.f_name as f_dishname"] = true;
    fColumnsVisible["bs.f_name as f_dishstate"] = true;
    fColumnsVisible["ob.f_removereason"] = true;
    fColumnsVisible["cst.f_name as f_storename"] = true;
    fColumnsVisible["sum(ob.f_qty2) as f_qty"] = true;
    fColumnsVisible["sum(ob.f_total) as f_total"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5SalesByDishesFilter(dbParams);
}

QToolBar *CR5SalesByDishes::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/menu.png"), tr("Dish"), this, SLOT(actionShowDish()));
    }
    return fToolBar;
}

void CR5SalesByDishes::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["oh.f_id as f_header"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_header"], 0);
    }
}

bool CR5SalesByDishes::tblDoubleClicked(int row, int column, const QList<QVariant> &v)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id as f_header"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (v.count() == 0) {
        return true;
    }
    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
    wo->setOrder(v.at(fModel->fColumnNameIndex["f_id"]).toString());
    return true;
}

void CR5SalesByDishes::actionShowDish()
{
    if (!fColumnsVisible["d.f_id as f_dishid"]) {
        C5Message::info(tr("Column 'Dish code' must be checked in filter"));
        return;
    }
    int r;
    if (!currentRow(r)) {
        return;
    }
    C5DishWidget *ep = new C5DishWidget(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, fModel->data(r, fModel->fColumnNameIndex["f_dishid"], Qt::EditRole).toInt());
    QList<QMap<QString, QVariant> > data;
    e->getResult(data);
    delete e;
}
