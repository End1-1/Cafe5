#include "cr5dishcomment.h"
#include "ce5dishcomment.h"

CR5DishComment::CR5DishComment(QWidget *parent) :
    C5ReportWidget( parent)
{
    fLabel = tr("Dish comment");
    fIconName = ":/menu.png";
    fSqlQuery = "select f_id, f_name from d_dish_comment ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5DishComment();
}

QToolBar *CR5DishComment::toolBar()
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
