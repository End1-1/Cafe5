#include "cr5dishpart2.h"

CR5DishPart2::CR5DishPart2(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Types of dishes");
    fSqlQuery = "select t.f_id, p.f_name as part_name, t.f_name, t.f_adgCode, \
                t.f_queue \
                from d_part2 t \
                left join d_part1 p on p.f_id=t.f_part ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["part_name"] = tr("Dept name");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_queue"] = tr("Queue");

    C5Cache *cd = createCache(cache_dish_part1);
    C5ComboDelegate *cbPart = new C5ComboDelegate("f_part", cd, fTableView);
    fTableView->setItemDelegateForColumn(1, cbPart);
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    QList<int> colsForUpdate;
    colsForUpdate << 2 << 3;
    setTableForUpdate("d_part2", colsForUpdate);
}

QToolBar *CR5DishPart2::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}
