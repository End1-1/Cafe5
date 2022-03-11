#include "cr5dishpart2.h"
#include "ce5dishpart2.h"
#include "c5tablemodel.h"
#include <QHeaderView>

CR5DishPart2::CR5DishPart2(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Types of dishes");
    fSqlQuery = "select t.f_id, p.f_name as part_name, p2.f_name as f_parentname, t.f_name, t.f_adgCode, \
                t.f_queue, t.f_color \
                from d_part2 t \
                left join d_part1 p on p.f_id=t.f_part \
                left join d_part2 p2 on p2.f_id=t.f_parent ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["part_name"] = tr("Dept name");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_queue"] = tr("Queue");
    fTranslation["f_color"] = tr("Color");
    fTranslation["f_parentname"] = tr("Parent");
    fEditor = new CE5DishPart2(dbParams);
}

QToolBar *CR5DishPart2::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
            fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(deletePart2()));
    }
    return fToolBar;
}

bool CR5DishPart2::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (C5ReportWidget::on_tblView_doubleClicked(index)) {
        fModel->setRowColor(index.row(), QColor::fromRgb(fModel->data(index.row(), fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
    return true;
}

void CR5DishPart2::buildQuery()
{
    C5Grid::buildQuery();
    setColors();
}

void CR5DishPart2::refreshData()
{
    C5Grid::refreshData();
    setColors();
}

void CR5DishPart2::setColors()
{
    fTableView->setColumnWidth(fModel->indexForColumnName("f_color"), 0);
    for (int i = 0, count = fModel->rowCount(); i < count; i++) {
        fModel->setRowColor(i, QColor::fromRgb(fModel->data(i, fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
}

void CR5DishPart2::deletePart2()
{
    int row = 0;
    int id = rowId(row, 0);
    if (id == 0) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_part"] = id;
    db.exec("select f_id from d_dish where f_part=:f_part");
    QList<int> ids;
    while (db.nextRow()) {
        ids << db.getInt(0);
    }
    foreach (int dishid, ids) {
        db[":f_dish"] = dishid;
        db.exec("select * from o_body where f_dish=:f_dish");
        if (db.nextRow()) {
            C5Message::error(tr("This name in use and cannot be removed"));
            return;
        }
    }
    if (C5Message::question(tr("Confirm to remove the selected dish part and all dishes that includes this part")) != QDialog::Accepted) {
        return;
    }
    foreach (int dishid, ids) {
        db[":f_dish"] = dishid;
        db.exec("delete from d_recipes where f_dish=:f_dish");
        db[":f_id"] = dishid;
        db.exec("delete from d_translator where f_id=:f_id");
        db[":f_dish"] = dishid;
        db.exec("delete from d_menu where f_dish=:f_dish");
    }
    db[":f_part"] = id;
    db.exec("delete from d_dish where f_part=:f_part");
    db[":f_id"] = id;
    db.exec("delete from d_part2 where f_id=:f_id");
    fModel->removeRow(row);
    C5Message::info(tr("Deleted"));
}
