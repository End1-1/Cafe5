#include "cr5cashnames.h"
#include "c5cashname.h"

CR5CashNames::CR5CashNames(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/cash.png";
    fLabel = tr("Cashboxes");
    fSqlQuery = "select c.f_id, c.f_name, c.f_currency_id, cur.f_name as f_currencyname "
                "from cash_box c "
                "left join e_currency cur on cur.f_id=c.f_currency_id";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_currency_id"] = tr("Currency code");
    fTranslation["f_currencyname"] = tr("Currency");
    fEditor = new C5CashName();
}

QToolBar *CR5CashNames::toolBar()
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
