#include "cr5currencycrossratehistory.h"
#include "ce5currencycrossraterecord.h"
#include "cr5currencycrossratehistoryfilter.h"

CR5CurrencyCrossRateHistory::CR5CurrencyCrossRateHistory(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/cash.png";
    fLabel = tr("Cross rates history");
    fMainTable = "e_currency_cross_rate_history cr";
    fLeftJoinTables << "left join e_currency ca on ca.f_id=cr.f_currency1 [ca]";
    fLeftJoinTables << "left join e_currency cb on cb.f_id=cr.f_currency2 [cb]";
    fSimpleQuery = false;
    fColumnsFields << "cr.f_id"
                   << "cr.f_date"
                   << "cr.f_currency1"
                   << "ca.f_name as f_currencyname1"
                   << "cr.f_currency2"
                   << "cb.f_name as f_currencyname2"
                   << "cr.f_rate";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_currency1"] = tr("Currency code 1");
    fTranslation["f_currencyname1"] = tr("Currency 1");
    fTranslation["f_currency2"] = tr("Currency code 2");
    fTranslation["f_currencyname2"] = tr("Currency 2");
    fTranslation["f_rate"] = tr("Rate");
    fColumnsVisible["cr.f_id"] = true;
    fColumnsVisible["cr.f_date"] = true;
    fColumnsVisible["cr.f_currency1"] = true;
    fColumnsVisible["ca.f_name as f_currencyname1"] = true;
    fColumnsVisible["cr.f_currency2"] = true;
    fColumnsVisible["cb.f_name as f_currencyname2"] = true;
    fColumnsVisible["cr.f_rate"] = true;
    fEditor = new CE5CurrencyCrossRateRecord();
    fFilterWidget = new CR5CurrencyCrossRateHistoryFilter();
}

QToolBar* CR5CurrencyCrossRateHistory::toolBar()
{
    if(!fToolBar) {
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
