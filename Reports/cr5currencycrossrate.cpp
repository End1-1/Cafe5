#include "cr5currencycrossrate.h"
#include "ce5currencycrossrate.h"

CR5CurrencyCrossRate::CR5CurrencyCrossRate(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cross rates");
    fSqlQuery = "select cr.f_id, cr.f_currency1, c1.f_name as f_currname1, cr.f_currency2, c2.f_name as f_currname2,"
                "cr.f_rate from e_currency_cross_rate cr "
                "left join e_currency c1 on c1.f_id=cr.f_currency1 "
                "left join e_currency c2 on c2.f_id=cr.f_currency2 ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_currency1"] = tr("Currency 1");
    fTranslation["f_currname1"] = tr("Name 1");
    fTranslation["f_currency2"] = tr("Currency 2");
    fTranslation["f_currname2"] = tr("Name 2");
    fTranslation["f_rate"] = tr("Rate");
    fTranslation["f_symbol"] = tr("Symbol");
    fEditor = new CE5CurrencyCrossRate();
}

QToolBar *CR5CurrencyCrossRate::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}
