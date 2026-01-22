#include "cr5carvisits.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5tablemodel.h"
#include "c5database.h"
#include "c5salefromstoreorder.h"
#include "cr5carvisitsfilter.h"
#include "c5message.h"

CR5CarVisits::CR5CarVisits(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/graph.png";
    fLabel = tr("Car visits");
    fSimpleQuery = false;
    fMainTable = "o_header oh";
    fLeftJoinTables << "left join o_header_options o on o.f_id=oh.f_id [o]"
                    << "left join b_car car on car.f_id=o.f_car [car]"
                    << "left join c_partners bc on bc.f_id=car.f_costumer [bc]"
                    << "left join s_car cm on cm.f_id=car.f_car [cm]";
    fColumnsFields << "oh.f_id"
                   << "oh.f_datecash"
                   << "concat(oh.f_prefix, oh.f_hallid) as orderid"
                   << "bc.f_contact"
                   << "cm.f_name"
                   << "car.f_govnumber"
                   << "sum(oh.f_amounttotal) as f_total"
                   << "sum(oh.f_amountcash) as f_cash"
                   << "sum(oh.f_amountcard) as f_card"
                   << "sum(oh.f_amountbank) as f_bank"
                   << "sum(oh.f_amountother) as f_other"
                   << "sum(oh.f_amountdiscount) as f_discount";
    fColumnsGroup << "oh.f_id"
                  << "oh.f_datecash"
                  << "concat(oh.f_prefix, oh.f_hallid) as orderid"
                  << "bc.f_contact"
                  << "cm.f_name"
                  << "car.f_govnumber";
    fColumnsSum << "f_total"
                << "f_cash"
                << "f_card"
                << "f_bank"
                << "f_other"
                << "f_discount";
    fTranslation["f_id"] = tr("UUID");
    fTranslation["f_datecash"] = tr("Date");
    fTranslation["orderid"] = tr("Order id");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_name"] = tr("Model");
    fTranslation["f_govnumber"] = tr("Gov number");
    fTranslation["f_total"] = tr("Total");
    fTranslation["f_cash"] = tr("Cash");
    fTranslation["f_card"] = tr("Card");
    fTranslation["f_bank"] = tr("Bank");
    fTranslation["f_other"] = tr("Other");
    fTranslation["f_discount"] = tr("Discount");
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as orderid"] = true;
    fColumnsVisible["bc.f_contact"] = true;
    fColumnsVisible["cm.f_name"] = true;
    fColumnsVisible["car.f_govnumber"] = true;
    fColumnsVisible["sum(oh.f_amounttotal) as f_total"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_cash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_card"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_bank"] = true;
    fColumnsVisible["sum(oh.f_amountother) as f_other"] = true;
    fColumnsVisible["sum(oh.f_amountdiscount) as f_discount"] = true;
    restoreColumnsVisibility();
    fFilterWidget = new CR5CarVisitsFilter();
}

QToolBar* CR5CarVisits::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbEdit
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }

    return fToolBar;
}

void CR5CarVisits::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();

    if(fColumnsVisible["oh.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5CarVisits::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }

    if(values.count() == 0) {
        C5Message::info(tr("Nothing selected"));
        return true;
    }

    C5Database db;
    db[":f_id"] = values.at(fModel->fColumnNameIndex["f_id"]).toString();
    db.exec("select f_source from o_header where f_id=:f_id");

    if(db.nextRow()) {
        switch(abs(db.getInt(0))) {
        case 1: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }

        case 2: {
            C5SaleFromStoreOrder::openOrder(mUser, values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }

        default: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        }
    } else {
        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
        wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
    }

    return true;
}
