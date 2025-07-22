#include "cr5dishremovereason.h"
#include "ce5dishremovereason.h"

CR5DishRemoveReason::CR5DishRemoveReason(QWidget *parent) :
    C5ReportWidget( parent)
{
    fLabel = tr("Dish remove reason");
    fIcon = ":/menu.png";
    fSqlQuery = "select f_id, f_name from o_dish_remove_reason";
    fSimpleQuery = true;
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fColumnsVisible["f_id"] = true;
    fColumnsVisible["f_name"] = true;
    restoreColumnsVisibility();
    fEditor = new CE5DishRemoveReason();
}

QToolBar *CR5DishRemoveReason::toolBar()
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
