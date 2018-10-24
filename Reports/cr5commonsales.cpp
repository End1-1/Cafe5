#include "cr5commonsales.h"
#include "cr5commonsalesfilter.h"

CR5CommonSales::CR5CommonSales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales, common");
    fSimpleQuery = false;

    fMainTable = "o_header oh";
    fLeftJoinTables << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                    << "left join h_tables ht on ht.f_id=oh.f_table [ht]"
                       ;

    fColumnsFields << "oh.f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                   << "sum(oh.f_amountcash) as f_amountcash"
                   << "sum(oh.f_amountcard) as f_amountcard"
                   << "sum(oh.f_amountbank) as f_amountbank"
                   << "sum(oh.f_amountother) as f_amountother"
                      ;

    fColumnsGroup << "oh.f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_amountcash"
                << "f_amountcard"
                << "f_amountbank"
                << "f_amountother"
                      ;

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_tablename"] = tr("Table");
    fTranslation["f_amounttotal"] = tr("Total");
    fTranslation["f_amountcash"] = tr("Cash");
    fTranslation["f_amountcard"] = tr("Card");
    fTranslation["f_amountbank"] = tr("Bank");
    fTranslation["f_amountother"] = tr("Other");

    fColumnsVisible["oh.f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["ht.f_name as f_tablename"] = true;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_amountcash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_amountcard"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_amountbank"] = true;
    fColumnsVisible["sum(oh.f_amountother) as f_amountother"] = true;

    restoreColumnsVisibility();

    fFilterWidget = new CR5CommonSalesFilter();
}

QToolBar *CR5CommonSales::toolBar()
{
    QList<ToolBarButtons> btn;
    btn << ToolBarButtons::tbEdit
        << ToolBarButtons::tbFilter
        << ToolBarButtons::tbClearFilter
        << ToolBarButtons::tbRefresh
        << ToolBarButtons::tbExcel
        << ToolBarButtons::tbPrint;
    QToolBar *tb = createStandartToolbar(btn);
    return tb;
}

void CR5CommonSales::editRow(int columnWidthId)
{
    Q_UNUSED(columnWidthId);
    C5Grid::editRow(1);
}
