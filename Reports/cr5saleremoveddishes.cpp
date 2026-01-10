#include "cr5saleremoveddishes.h"
#include "c5tablemodel.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "cr5saleremoveddishesfilter.h"
#include "c5message.h"

CR5SaleRemovedDishes::CR5SaleRemovedDishes(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/delete.png";
    fLabel = tr("Sales, removed dishes");
    fSimpleQuery = false;
    fMainTable = "o_body ob";
    fLeftJoinTables << "left join o_header oh on oh.f_id=ob.f_header [oh]"
                    << "left join h_halls h on h.f_id=oh.f_hall [h]"
                    << "left join h_tables t on t.f_id=oh.f_table [t]"
                    << "left join o_state os on os.f_id=oh.f_state [os]"
                    << "left join o_body_state bs on bs.f_id=ob.f_state [bs]"
                    << "left join d_dish d on d.f_id=ob.f_dish [d]"
                    << "left join d_part2 dp on dp.f_id=d.f_part [dp]"
                    ;
    fColumnsFields << "oh.f_id as f_header"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_order"
                   << "os.f_name as f_statename"
                   << "h.f_name as f_hallname"
                   << "t.f_name as f_tablename"
                   << "oh.f_datecash"
                   << "ob.f_removetime"
                   << "dp.f_name as f_dishpart"
                   << "d.f_name as f_dishname"
                   << "if(ob.f_removeprecheck<>0, 'Այո', 'Ոչ') AS f_removeprecheck"
                   << "bs.f_name as f_dishstate"
                   << "ob.f_removereason"
                   << "ob.f_qty2 as f_qty"
                   << "ob.f_total as f_total"
                   ;
    fColumnsSum << "f_qty"
                << "f_total"
                ;
    fTranslation["f_header"] = tr("Header");
    fTranslation["f_order"] = tr("Order");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_tablename"] = tr("Table");
    fTranslation["f_statename"] = tr("Order state");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_removetime"] = tr("Removed");
    fTranslation["f_dishpart"] = tr("Type of dish");
    fTranslation["f_dishname"] = tr("Dish");
    fTranslation["f_removeprecheck"] = tr("Precheck");
    fTranslation["f_dishstate"] = tr("Dish state");
    fTranslation["f_removereason"] = tr("Remove reason");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_total"] = tr("Total");
    fColumnsVisible["oh.f_id as f_header"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["ob.f_removetime"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_order"] = true;
    fColumnsVisible["h.f_name as f_hallname"] = true;
    fColumnsVisible["t.f_name as f_tablename"] = true;
    fColumnsVisible["dp.f_name as f_dishpart"] = true;
    fColumnsVisible["d.f_name as f_dishname"] = true;
    fColumnsVisible["if(ob.f_removeprecheck<>0, 'Այո', 'Ոչ') AS f_removeprecheck"] = true;
    fColumnsVisible["bs.f_name as f_dishstate"] = true;
    fColumnsVisible["ob.f_removereason"] = true;
    fColumnsVisible["sum(ob.f_qty2) as f_qty"] = true;
    fColumnsVisible["sum(ob.f_total) as f_total"] = true;
    restoreColumnsVisibility();
    fFilterWidget = new CR5SaleRemovedDishesFilter();
    fFilter = static_cast<CR5SaleRemovedDishesFilter*>(fFilterWidget);
}

QToolBar* CR5SaleRemovedDishes::toolBar()
{
    if(!fToolBar) {
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

void CR5SaleRemovedDishes::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();

    if(fColumnsVisible["oh.f_id as f_header"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_header"], 0);
    }
}

bool CR5SaleRemovedDishes::tblDoubleClicked(int row, int column, const QVector<QJsonValue>& v)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(!fColumnsVisible["oh.f_id as f_header"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }

    if(v.count() == 0) {
        return true;
    }

    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
    wo->setOrder(v.at(fModel->fColumnNameIndex["f_header"]).toString());
    return true;
}
