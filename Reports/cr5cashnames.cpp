#include "cr5cashnames.h"
#include "c5cashname.h"

CR5CashNames::CR5CashNames(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cash names");
    fSqlQuery = "select f_id, f_name from e_cash_names";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
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
