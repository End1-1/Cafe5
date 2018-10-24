#include "cr5dishpart1.h"

CR5DishPart1::CR5DishPart1(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Dish depts");
    fSqlQuery = "select f_id, f_name from d_part1";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    QList<int> colsForUpdate;
    colsForUpdate << 1;
    setTableForUpdate("d_part1", colsForUpdate);
}

QToolBar *CR5DishPart1::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}
