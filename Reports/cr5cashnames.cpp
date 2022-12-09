#include "cr5cashnames.h"
#include "c5cashname.h"

CR5CashNames::CR5CashNames(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cash names");
    fSqlQuery = "select c.f_id, c.f_name, c.f_currency, cr.f_name  as f_currencyname "
                "from e_cash_names c "
                "left join e_currency cr on cr.f_id=c.f_currency";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_currency"] = tr("Currency code");
    fTranslation["f_currencyname"] = tr("Currency");
    fEditor = new C5CashName(dbParams);
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
