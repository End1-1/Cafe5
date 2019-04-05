#include "cr5commonsales.h"
#include "cr5commonsalesfilter.h"
#include "c5tablemodel.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"

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
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                   << "sum(oh.f_amountcash) as f_amountcash"
                   << "sum(oh.f_amountcard) as f_amountcard"
                   << "sum(oh.f_amountbank) as f_amountbank"
                   << "sum(oh.f_amountother) as f_amountother"
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
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_amountcash"
                << "f_amountcard"
                << "f_amountbank"
                << "f_amountother"
                      ;

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
    fTranslation["f_tablename"] = tr("Table");
    fTranslation["f_amounttotal"] = tr("Total");
    fTranslation["f_amountcash"] = tr("Cash");
    fTranslation["f_amountcard"] = tr("Card");
    fTranslation["f_amountbank"] = tr("Bank");
    fTranslation["f_amountother"] = tr("Other");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["ht.f_name as f_tablename"] = true;
    fColumnsVisible["f_dateopen"] = false;
    fColumnsVisible["f_dateclose"] = false;
    fColumnsVisible["f_timeopen"] = false;
    fColumnsVisible["f_timeclose"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_staff"] = false;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_amountcash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_amountcard"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_amountbank"] = true;
    fColumnsVisible["sum(oh.f_amountother) as f_amountother"] = true;

    connect(this, SIGNAL(tblDoubleClicked(int, int, QList<QVariant>)), this, SLOT(openOrder(int, int, QList<QVariant>)));

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

void CR5CommonSales::openOrder(int row, int column, const QList<QVariant> &values)
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
