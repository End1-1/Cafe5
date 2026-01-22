#include "cr5currencies.h"
#include "ce5currency.h"

CR5Currencies::CR5Currencies(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/cash.png";
    fLabel = tr("Currencies");
    fSqlQuery = "select f_id, f_name, f_rate, f_symbol from e_currency";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_rate"] = tr("Rate");
    fTranslation["f_symbol"] = tr("Symbol");
    fEditor = new CE5Currency();
}

QToolBar *CR5Currencies::toolBar()
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
