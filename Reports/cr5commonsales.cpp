#include "cr5commonsales.h"
#include "cr5commonsalesfilter.h"
#include "c5tablemodel.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5salefromstoreorder.h"
#include "c5waiterorderdoc.h"

CR5CommonSales::CR5CommonSales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales, common");
    fSimpleQuery = false;

    fMainTable = "o_header oh";
    fLeftJoinTables << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                    << "left join h_tables ht on ht.f_id=oh.f_table [ht]"
                    << "left join s_user w on w.f_id=oh.f_staff [w]"
                    << "left join o_state os on os.f_id=oh.f_state [os]"
                    << "left join c_partners cpb on cpb.f_id=oh.f_partner [cpb]"
                       ;

    fColumnsFields << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "os.f_name as f_statename"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                   << "oh.f_dateopen"
                   << "oh.f_timeopen"
                   << "oh.f_dateclose"
                   << "oh.f_timeclose"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                   << "sum(oh.f_amountcash) as f_amountcash"
                   << "sum(oh.f_amountcard) as f_amountcard"
                   << "sum(oh.f_amountbank) as f_amountbank"
                   << "sum(oh.f_amountother) as f_amountother"
                   << "oh.f_amountservice"
                   << "oh.f_amountdiscount"
                      ;

    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "os.f_name as f_statename"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "oh.f_dateopen"
                   << "oh.f_timeopen"
                   << "oh.f_dateclose"
                   << "oh.f_timeclose"
                   << "oh.f_amountservice"
                   << "oh.f_amountdiscount"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_amountcash"
                << "f_amountcard"
                << "f_amountbank"
                << "f_amountother"
                << "f_amountservice"
                << "f_amountdiscount"
                      ;

    fColumnsOrder << "oh.f_datecash"
                  << "oh.f_timeclose";

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_staff"] = tr("Staff");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_dateopen"] = tr("Open date");
    fTranslation["f_dateclose"] = tr("Close date");
    fTranslation["f_timeopen"] = tr("Open time");
    fTranslation["f_timeclose"] = tr("Close time");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_buyer"] = tr("Buyer");
    fTranslation["f_buyertaxcode"] = tr("Buyer taxcode");
    if (__c5config.frontDeskMode() == FRONTDESK_SHOP) {
        fTranslation["f_tablename"] = tr("Cash");
    } else {
        fTranslation["f_tablename"] = tr("Table");
    }
    fTranslation["f_amounttotal"] = tr("Total");
    fTranslation["f_amountcash"] = tr("Cash");
    fTranslation["f_amountcard"] = tr("Card");
    fTranslation["f_amountbank"] = tr("Bank");
    fTranslation["f_amountother"] = tr("Other");
    fTranslation["f_amountservice"] = tr("Service");
    fTranslation["f_amountdiscount"] = tr("Discount");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["ht.f_name as f_tablename"] = true;
    fColumnsVisible["oh.f_dateopen"] = false;
    fColumnsVisible["oh.f_dateclose"] = false;
    fColumnsVisible["oh.f_timeopen"] = false;
    fColumnsVisible["oh.f_timeclose"] = false;
    fColumnsVisible["cpb.f_taxname as f_buyer"] = false;
    fColumnsVisible["cpb.f_taxcode as f_buyertaxcode"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_staff"] = false;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_amountcash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_amountcard"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_amountbank"] = true;
    fColumnsVisible["sum(oh.f_amountother) as f_amountother"] = true;
    fColumnsVisible["oh.f_amountservice"] = false;
    fColumnsVisible["oh.f_amountdiscount"] = false;

    restoreColumnsVisibility();

    fFilterWidget = new CR5CommonSalesFilter(fDBParams);
}

QToolBar *CR5CommonSales::toolBar()
{
    if (!fToolBar) {
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

void CR5CommonSales::editRow(int columnWidthId)
{
    Q_UNUSED(columnWidthId);
    C5Grid::editRow(1);
}

void CR5CommonSales::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["oh.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5CommonSales::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (values.count() == 0) {
        C5Message::info(tr("Nothing selected"));
        return true;
    }
    C5Database db(fDBParams);
    db[":f_id"] = values.at(fModel->fColumnNameIndex["f_id"]).toString();
    db.exec("select f_source from o_header where f_id=:f_id");
    if (db.nextRow()) {
        switch (abs(db.getInt(0))) {
        case 1: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        case 2: {
            C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        default: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        }
    } else {
        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
        wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
    }
    return true;
}

void CR5CommonSales::transferToRoom()
{
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return;
    }
    QModelIndexList ml = this->fTableView->selectionModel()->selectedIndexes();
    QSet<int> rows;
    foreach (QModelIndex m, ml) {
        rows << m.row();
    }
    C5Database db1(fDBParams);
    C5Database db2(fDBParams.at(0), C5Config::hotelDatabase(), fDBParams.at(2), fDBParams.at(3));
    foreach (int r, rows) {
        QString err;
        C5WaiterOrderDoc w(fModel->data(r, fModel->fColumnNameIndex["f_id"], Qt::EditRole).toString(), db1);
        w.transferToHotel(db1, db2, err);
        if (!err.isEmpty()) {
            C5Message::error(err + "<br>" + fModel->data(r, 0, Qt::EditRole).toString());
            return;
        }
    }
    C5Message::info(tr("Done"));
}
