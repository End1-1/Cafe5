#include "cr5dishpriceselfcost.h"
#include "cr5dishpriceselfcostfilter.h"
#include "c5tablemodel.h"
#include "c5dishwidget.h"

CR5DishPriceSelfCost::CR5DishPriceSelfCost(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Dish prices and self cost");
    fIcon = ":/menu.png";
    fFilterWidget = new CR5DishPriceSelfCostFilter(dbParams);
    fFilter = static_cast<CR5DishPriceSelfCostFilter*>(fFilterWidget);
    fSimpleQuery = true;
}

QToolBar *CR5DishPriceSelfCost::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5DishPriceSelfCost::buildQuery()
{
    if (fFilter->isSimpleView()) {
        buildQueryV1();
    } else {
        buildQueryV2();
    }
}

void CR5DishPriceSelfCost::buildQueryV1()
{
    fTableView->clearSpans();
    fSqlQuery = "select d.f_id, p2.f_name as f_part, d.f_name, sum(r.f_qty*r.f_price) as f_selfcost "
                "from d_recipes r "
                "left join d_dish d on d.f_id=r.f_dish "
                "left join d_part2 p2 on p2.f_id=d.f_part "
                "left join d_menu m on m.f_dish=d.f_id "
                "where d.f_id>0 ";
    switch (fFilter->menuState()) {
    case 0:
        fSqlQuery += " and m.f_state=0 ";
        break;
    case 1:
        fSqlQuery += " and m.f_state=1 ";
        break;
    }

    if (fFilter->baseOnSale()) {
        fSqlQuery += QString(" and r.f_dish in (select distinct(b.f_dish) "
                             "from o_body b "
                             "left join o_header h on h.f_id=b.f_header "
                             "where h.f_datecash between '%1' and '%2') ")
            .arg(fFilter->d1().toString(FORMAT_DATE_TO_STR_MYSQL))
            .arg(fFilter->d2().toString(FORMAT_DATE_TO_STR_MYSQL));
    }
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
        menuid = "where m.f_id in(" + fFilter->menuId() + ") ";
    }
    QMap<int, int> menuMap;
    db.exec("select m.f_id, m.f_name from d_menu_names m " + menuid);
    while (db.nextRow()) {
        menuMap[db.getInt(0)] = fModel->columnCount();
        fModel->insertColumn(fModel->columnCount(), db.getString(1) + ", " + tr("price"));
        fModel->insertColumn(fModel->columnCount(), db.getString(1) + ", " + tr("factor"));
    }
    QString query = "select mn.f_id, mn.f_name, m.f_dish, m.f_price from d_menu m left join d_menu_names mn on mn.f_id=m.f_menu " + (fFilterWidget->condition().isEmpty() ? "" : " where " + fFilterWidget->condition());
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

void CR5DishPriceSelfCost::buildQueryV2()
{
    buildQueryV1();
    fModel->insertColumn(3, tr("Code"));
    fModel->insertColumn(4, tr("Goods name"));
    fModel->insertColumn(5, tr("Qty"));
    fModel->insertColumn(6, tr("Unit"));
    fModel->insertColumn(7, tr("Price"));
    fModel->insertColumn(8, tr("Cost"));
    C5Database db(fDBParams);
    QString query = "select r.f_goods, g.f_name, r.f_qty, u.f_name, r.f_price, r.f_price*r.f_qty, r.f_dish "
            "from d_recipes r "
            "left join c_goods g on g.f_id=r.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "order by r.f_dish ";

    db.exec(query);
    int currDish = 0;
    int currRow = 0;
    int totalRows = 1;
    QMap<int, int> dishMap;
    while (db.nextRow()) {
        if (db.getInt(6) != currDish) {
            dishMap.clear();
            for (int i = 0; i < fModel->rowCount(); i++) {
                dishMap[fModel->data(i, 0, Qt::EditRole).toInt()] = i;
            }
            dishMap.remove(0);
            if (!dishMap.contains(db.getInt(6))) {
                continue;
            }
            currRow = dishMap[db.getInt(6)];
            currDish = db.getInt(6);
            totalRows = 0;
        }
        fModel->insertRow(currRow + totalRows);
        fModel->setData(currRow + totalRows + 1, 3, db.getInt(0));
        fModel->setData(currRow + totalRows + 1, 4, db.getString(1));
        fModel->setData(currRow + totalRows + 1, 5, db.getDouble(2));
        fModel->setData(currRow + totalRows + 1, 6, db.getString(3));
        fModel->setData(currRow + totalRows + 1, 7, db.getDouble(4));
        fModel->setData(currRow + totalRows + 1, 8, db.getDouble(5));
        totalRows++;
    }
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (fModel->data(i, 0, Qt::EditRole).toInt() > 0) {
            fTableView->setSpan(i, 3, 1, 6);
            fModel->setData(i, 3, tr("Recipe") + " " + fModel->data(i, 2, Qt::EditRole).toString());
        }
    }
}

bool CR5DishPriceSelfCost::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (values.count() == 0) {
        return true;
    }
    if (values.at(0).toInt() == 0) {
        return true;
    }
    C5DishWidget *ep = new C5DishWidget(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, values.at(0).toInt());
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {

    }
    delete e;
    return true;
}
