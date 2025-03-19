#include "cr5commonsales.h"
#include "cr5commonsalesfilter.h"
#include "c5tablemodel.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5saledoc.h"
#include "c5waiterorderdoc.h"
#include "dlgexportsaletoasoptions.h"
#include "c5dlgselectreporttemplate.h"
#include "c5salefromstoreorder.h"
#include "c5gridgilter.h"
#include <QDir>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

CR5CommonSales::CR5CommonSales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales by tickets");
    fSimpleQuery = false;
    fMainTable = "o_header oh";
    fLeftJoinTables << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                    << "left join h_tables ht on ht.f_id=oh.f_table [ht]"
                    << "left join o_header_flags ohf on ohf.f_id=oh.f_id [ohf]"
                    << "left join s_user w on w.f_id=oh.f_staff [w]"
                    << "left join s_user ww on ww.f_id=oh.f_cashier [ww]"
                    << "left join o_state os on os.f_id=oh.f_state [os]"
                    << "left join o_tax ot on ot.f_id=oh.f_id [ot]"
                    << "left join c_partners cpb on cpb.f_id=oh.f_partner [cpb]"
                    << "left join a_header_cash ahc on ahc.f_oheader=oh.f_id [ahc]"
                    << "left join e_cash ec on ec.f_header=ahc.f_id and ec.f_sign=-1 [ec]"
                    ;
    fColumnsFields << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "os.f_name as f_statename"
                   << "oh.f_datecash"
                   << "dayofweek(oh.f_datecash) as f_dayofweek"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "concat_ws(' ', w.f_last, w.f_first) as f_cashier"
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                   << "oh.f_dateopen"
                   << "oh.f_timeopen"
                   << "oh.f_dateclose"
                   << "oh.f_timeclose"
                   << "concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "count(oh.f_id) as f_count"
                   << "ot.f_receiptnumber"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                   << "sum(oh.f_amountcash) as f_amountcash"
                   << "sum(oh.f_amountcard) as f_amountcard"
                   << "sum(oh.f_amountbank) as f_amountbank"
                   << "sum(oh.f_amountcredit) as f_amountcredit"
                   << "sum(oh.f_amountother) as f_amountother"
                   << "sum(oh.f_amountidram) as f_amountidram"
                   << "sum(oh.f_amounttelcell) as f_amounttelcell"
                   << "sum(oh.f_amountpayx) as f_amountpayx"
                   << "sum(oh.f_amountdebt) as f_amountdebt"
                   << "sum(oh.f_amountprepaid) as f_amountprepaid"
                   << "sum(oh.f_hotel) as f_hotel"
                   << "sum(ec.f_amount) as f_amountout"
                   << "oh.f_amountservice"
                   << "oh.f_amountdiscount"
                   << "oh.f_comment"
                   ;
    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                  << "oh.f_id"
                  << "oh.f_datecash"
                  << "dayofweek(oh.f_datecash) as f_dayofweek"
                  << "os.f_name as f_statename"
                  << "hl.f_name as f_hallname"
                  << "ht.f_name as f_tablename"
                  << "ot.f_receiptnumber"
                  << "oh.f_dateopen"
                  << "oh.f_timeopen"
                  << "oh.f_dateclose"
                  << "oh.f_timeclose"
                  << "oh.f_amountservice"
                  << "oh.f_amountdiscount"
                  << "concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"
                  << "cpb.f_taxcode as f_buyertaxcode"
                  << "concat_ws(' ', w.f_last, w.f_first) as f_cashier"
                  << "concat(w.f_last, ' ', w.f_first) as f_staff"
                  << "oh.f_comment"
                  ;
    fColumnsSum << "f_amounttotal"
                << "f_amountcash"
                << "f_amountcard"
                << "f_amountbank"
                << "f_amountcredit"
                << "f_amountother"
                << "f_amountidram"
                << "f_amountpayx"
                << "f_amountservice"
                << "f_amounttelcell"
                << "f_amountdiscount"
                << "f_amountprepaid"
                << "f_amountdebt"
                << "f_hotel"
                << "f_count"
                << "f_amountout"
                ;
    fColumnsOrder << "oh.f_datecash"
                  << "oh.f_timeclose";
    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_cashier"] = tr("Cashier");
    fTranslation["f_staff"] = tr("Staff");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_dateopen"] = tr("Open date");
    fTranslation["f_dateclose"] = tr("Close date");
    fTranslation["f_timeopen"] = tr("Open time");
    fTranslation["f_timeclose"] = tr("Close time");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_dayofweek"] = tr("Day of week");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_amountdebt"] = tr("Debt");
    fTranslation["f_amounttelcell"] = tr("Telcell");
    fTranslation["f_buyer"] = tr("Buyer");
    fTranslation["f_receiptnumber"] = tr("Tax");
    fTranslation["f_buyertaxcode"] = tr("Buyer taxcode");
    if (__c5config.frontDeskMode() == FRONTDESK_SHOP) {
        fTranslation["f_tablename"] = tr("Cashdesk");
    } else {
        fTranslation["f_tablename"] = tr("Table");
    }
    fTranslation["f_amounttotal"] = tr("Total");
    fTranslation["f_amountcash"] = tr("Cash");
    fTranslation["f_amountcard"] = tr("Card");
    fTranslation["f_amountbank"] = tr("Bank");
    fTranslation["f_amountcredit"] = tr("Credit");
    fTranslation["f_amountother"] = tr("Other");
    fTranslation["f_amountprepaid"] = tr("Prepaid");
    fTranslation["f_amountidram"] = tr("Idram");
    fTranslation["f_amountpayx"] = tr("PayX");
    fTranslation["f_amountservice"] = tr("Service");
    fTranslation["f_amountdiscount"] = tr("Discount");
    fTranslation["f_comment"] = tr("Comment");
    fTranslation["f_count"] = tr("Count");
    fTranslation["f_hotel"] = tr("Hotel");
    fTranslation["f_amountout"] = tr("Out");
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["dayofweek(oh.f_datecash) as f_dayofweek"] = false;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["ht.f_name as f_tablename"] = true;
    fColumnsVisible["oh.f_dateopen"] = false;
    fColumnsVisible["oh.f_dateclose"] = false;
    fColumnsVisible["oh.f_timeopen"] = false;
    fColumnsVisible["oh.f_timeclose"] = false;
    fColumnsVisible["sum(oh.f_amountdebt) as f_amountdebt"] = false;
    fColumnsVisible["sum(oh.f_amounttelcell) as f_amounttelcell"] = false;
    fColumnsVisible["concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"] = false;
    fColumnsVisible["cpb.f_taxcode as f_buyertaxcode"] = false;
    fColumnsVisible["concat_ws(' ', w.f_last, w.f_first) as f_cashier"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_staff"] = false;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_amountcash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_amountcard"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_amountbank"] = false;
    fColumnsVisible["sum(oh.f_amountcredit) as f_amountcredit"] = false;
    fColumnsVisible["sum(oh.f_amountother) as f_amountother"] = true;
    fColumnsVisible["sum(oh.f_amountidram) as f_amountidram"] = false;
    fColumnsVisible["sum(oh.f_amountpayx) as f_amountpayx"] = false;
    fColumnsVisible["sum(oh.f_amountprepaid) as f_amountprepaid"] = false;
    fColumnsVisible["sum(oh.f_hotel) as f_hotel"] = true;
    fColumnsVisible["oh.f_amountservice"] = false;
    fColumnsVisible["oh.f_amountdiscount"] = false;
    fColumnsVisible["oh.f_comment"] = false;
    fColumnsVisible["count(oh.f_id) as f_count"] = false;
    fColumnsVisible["ot.f_receiptnumber"] = false;
    fColumnsVisible["sum(ec.f_amount) as f_amountout"] = false;
    restoreColumnsVisibility();
    fFilterWidget = new CR5CommonSalesFilter(fDBParams);
    fFilter = static_cast<CR5CommonSalesFilter *>(fFilterWidget);
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
        auto *a = new QAction(QIcon(":/template.png"), tr("Templates"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(templates()));
        fToolBar->insertAction(fToolBar->actions().at(3), a);
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

bool CR5CommonSales::tblDoubleClicked(int row, int column, const QJsonArray &values)
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
    db.exec("select * from o_header where f_id=:f_id");
    if (db.nextRow()) {
        switch (abs(db.getInt("f_source"))) {
            case 1: {
                if (db.getDouble("f_amounttotal") > 0) {
                    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
                    wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
                } else {
                    C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->fColumnNameIndex["f_id"]).toString());
                }
                break;
            }
            case 2: {
                auto *doc = __mainWindow->createTab<C5SaleDoc>(fDBParams);
                doc->openDoc(values.at(fModel->fColumnNameIndex["f_id"]).toString());
                //C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->fColumnNameIndex["f_id"]).toString());
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
    C5Database db1(fDBParams);;
    foreach (int r, rows) {
        QString err;
        C5WaiterOrderDoc w(fModel->data(r, fModel->fColumnNameIndex["f_id"], Qt::EditRole).toString(), db1);
        w.transferToHotel(db1, err);
        if (!err.isEmpty()) {
            C5Message::error(err + "<br>" + fModel->data(r, 0, Qt::EditRole).toString());
            return;
        }
    }
    C5Message::info(tr("Done"));
}

void CR5CommonSales::templates()
{
    C5DlgSelectReportTemplate d(reporttemplate_commonsales, fDBParams);
    if (d.exec() == QDialog::Accepted) {
        QString condition;
        QMap<QString, bool> showColumns = fColumnsVisible;
        fColumnsVisible.clear();
        if (C5GridGilter::filter(fFilterWidget, condition, fColumnsVisible, fTranslation)) {
            QString sql = d.fSelectedTemplate.sql;
            sql.replace("%date1", fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL));
            sql.replace("%date2", fFilter->date2().toString(FORMAT_DATE_TO_STR_MYSQL));
            executeSql(sql);
            fTableView->resizeColumnsToContents();
        }
        fColumnsVisible = showColumns;
    }
}
