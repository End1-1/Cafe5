#include "cr5dish.h"
#include "c5dishwidget.h"

CR5Dish::CR5Dish(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/users_groups.png";
    fLabel = tr("Users");

    fSqlQuery = "select d.f_id, p1.f_name as f_part1, p2.f_name as f_part2, d.f_name, \
                f_remind, f_service, f_discount, d.f_queue \
                from d_dish d \
                left join d_part2 p2 on p2.f_id=d.f_part \
                left join d_part1 p1 on p1.f_id=p2.f_part ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_part1"] = tr("Dept");
    fTranslation["f_part2"] = tr("Type");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_remind"] = tr("Remind");
    fTranslation["f_service"] = tr("Service");
    fTranslation["f_discount"] = tr("Discount");

    C5Cache *cd = createCache(cache_dish_part2);
    C5ComboDelegate *cbPart = new C5ComboDelegate("f_part", cd, fTableView);
    fTableView->setItemDelegateForColumn(2, cbPart);
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(5, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(6, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(7, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 3 << 4 << 5 << 6 << 7;
    setTableForUpdate("d_dish", colsForUpdate);

    C5DishWidget *dw = new C5DishWidget(dbParams, this);
    hl()->addWidget(dw);
}

QToolBar *CR5Dish::toolBar()
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