#include "cr5creditcards.h"
#include "ce5creditcard.h"

CR5CreditCards::CR5CreditCards(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Credit cards");
    fIcon = ":/credit-card.png";
    fSimpleQuery = true;
    fSqlQuery = "select f_id, f_name from o_credit_card";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fColumnsVisible["f_id"] = true;
    fColumnsVisible["f_name"] = true;

    restoreColumnsVisibility();

    fEditor = new CE5CreditCard(dbParams);
}

QToolBar *CR5CreditCards::toolBar()
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
