#include "cr5currencyratehistory.h"
#include "ce5currencyrate.h"
#include "cr5currencyratehistoryfilter.h"

CR5CurrencyRateHistory::CR5CurrencyRateHistory(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Currency rates history");
    fMainTable = "e_currency_rate_history h";
    fLeftJoinTables << "left join e_currency c on c.f_id=h.f_currency [c]";
    fSimpleQuery = false;

    fColumnsFields << "h.f_id"
                   << "h.f_date"
                   << "h.f_currency"
                   << "c.f_name as f_currencyname"
                   << "h.f_rate";


    fTranslation["f_id"] = tr("Code");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_currency"] = tr("Currency code");
    fTranslation["f_currencyname"] = tr("Currency");
    fTranslation["f_rate"] = tr("Rate");

    fColumnsVisible["h.f_id"] = true;
    fColumnsVisible["h.f_date"] = true;
    fColumnsVisible["h.f_currency"] = true;
    fColumnsVisible["c.f_name as f_currencyname"] = true;
    fColumnsVisible["h.f_rate"] = true;

    fEditor = new CE5CurrencyRate();
    fFilterWidget = new CR5CurrencyRateHistoryFilter();
}

QToolBar *CR5CurrencyRateHistory::toolBar()
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
