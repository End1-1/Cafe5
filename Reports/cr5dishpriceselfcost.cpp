#include "cr5dishpriceselfcost.h"
#include "cr5dishpriceselfcostfilter.h"
#include "c5tablemodel.h"

CR5DishPriceSelfCost::CR5DishPriceSelfCost(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Dish prices and self cost");
    fIcon = ":/menu.png";
    fFilterWidget = new CR5DishPriceSelfCostFilter(dbParams);
    fSimpleQuery = true;
}

QToolBar *CR5DishPriceSelfCost::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5DishPriceSelfCost::buildQuery()
{
    fSqlQuery = "select d.f_id, p2.f_name as f_part, d.f_name, sum(r.f_qty*r.f_price) as f_selfcost \
                from d_recipes r \
                left join d_dish d on d.f_id=r.f_dish \
                left join d_part2 p2 on p2.f_id=d.f_part \
                left join d_menu m on m.f_dish=d.f_id \
                where m.f_menu=1 and m.f_state=1 ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_part"] = tr("Part");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_selfcost"] = tr("Self cost");
    if (!fFilterWidget->condition().isEmpty()) {
        fSqlQuery += " and " + fFilterWidget->condition();
    }
    fSqlQuery += " group by 1, 2, 3";
    fSqlQuery += " order by 2, 3 ";
    C5Grid::buildQuery();
    QMap<int, int> dishMap;
    for (int i = 0; i < fModel->rowCount(); i++) {
        dishMap[fModel->data(i, 0, Qt::DisplayRole).toInt()] = i;
    }
    C5Database db(fDBParams);
    QString menuid;
    if (!fFilterWidget->condition().isEmpty()) {
        menuid = "where " + fFilterWidget->condition().isEmpty();
    }
    QMap<int, int> menuMap;
    db.exec("select m.f_id, m.f_name from d_menu_names m " + menuid);
    while (db.nextRow()) {
        menuMap[db.getInt(0)] = fModel->columnCount();
        fModel->insertColumn(fModel->columnCount(), db.getString(1) + ", " + tr("price"));
        fModel->insertColumn(fModel->columnCount(), db.getString(1) + ", " + tr("factor"));
    }
    QString query = "select mn.f_id, mn.f_name, m.f_dish, m.f_price from d_menu m left join d_menu_names mn on mn.f_id=m.f_menu " + menuid;
    db.exec(query);
    while (db.nextRow()) {
        if (!dishMap.contains(db.getInt(2))) {
            continue;
        }
        if (!menuMap.contains(db.getInt(0))) {
            continue;
        }
        fModel->setData(dishMap[db.getInt(2)], menuMap[db.getInt(0)], db.getDouble(3), Qt::EditRole);
        if (fModel->data(dishMap[db.getInt(2)], 3, Qt::EditRole).toDouble() > 0.0001) {
            fModel->setData(dishMap[db.getInt(2)], menuMap[db.getInt(0)] + 1, db.getDouble(3) / fModel->data(dishMap[db.getInt(2)], 3, Qt::EditRole).toDouble(), Qt::EditRole);
        } else {
            fModel->setData(dishMap[db.getInt(2)], menuMap[db.getInt(0)] + 1, 0, Qt::EditRole);
        }
    }
}
