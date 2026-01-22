#include "cr5dishpart1.h"
#include "ce5dishpart1.h"

CR5DishPart1::CR5DishPart1(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIconName = ":/menu.png";
    fLabel = tr("Dish depts");
    fSqlQuery = "select f_id, f_name from d_part1";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5DishPart1();
}

QToolBar *CR5DishPart1::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}
