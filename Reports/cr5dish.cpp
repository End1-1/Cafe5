#include "cr5dish.h"
#include "c5dishwidget.h"
#include "c5mainwindow.h"
#include "c5tablemodel.h"
#include "cr5menutranslator.h"
#include <QAbstractScrollArea>

CR5Dish::CR5Dish(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Dishes");

    fSqlQuery = "select d.f_id, p1.f_name as f_part1, p2.f_name as f_part2, d.f_name, \
                f_remind, f_service, f_discount, d.f_queue, d.f_color, f_netweight, f_cost \
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
    fTranslation["f_queue"] = tr("Queue");
    fTranslation["f_color"] = tr("Color");
    fTranslation["f_netweight"] = tr("Weight");
    fTranslation["f_cost"] = tr("Cost");
    fEditor = new C5DishWidget(dbParams);
}

QToolBar *CR5Dish::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/translate.png"), tr("Translator"), this, SLOT(translator()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(deleteDish()));
        auto *g = new QAction(QIcon(":/goodsback.png"), tr("Output to AS"));
        connect(g, SIGNAL(triggered(bool)), this, SLOT(asoutput(bool)));
        fToolBar->addAction(g);
    }
    return fToolBar;
}

bool CR5Dish::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (C5Grid::on_tblView_doubleClicked(index)) {
        fModel->setRowColor(index.row(), QColor::fromRgb(fModel->data(index.row(), fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
    return true;
}

void CR5Dish::buildQuery()
{
    C5Grid::buildQuery();
    setColors();
}

void CR5Dish::refreshData()
{
    C5Grid::refreshData();
    setColors();
}

void CR5Dish::setColors()
{
    fTableView->setColumnWidth(fModel->indexForColumnName("f_color"), 0);
    for (int i = 0, count = fModel->rowCount(); i < count; i++) {
        fModel->setRowColor(i, QColor::fromRgb(fModel->data(i, fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
}

void CR5Dish::translator()
{
    __mainWindow->createTab<CR5MenuTranslator>(fDBParams);
}

void CR5Dish::deleteDish()
{
    int row = 0;
    int id = rowId(row, 0);
    if (id == 0) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_dish"] = id;
    db.exec("select * from o_body where f_dish=:f_dish");
    if (db.nextRow()) {
        C5Message::error(tr("This name in use and cannot be removed"));
        return;
    }
    if (C5Message::question(tr("Confirm to remove the selected dish")) != QDialog::Accepted) {
        return;
    }
    db[":f_dish"] = id;
    db.exec("delete from d_recipes where f_dish=:f_dish");
    db[":f_id"] = id;
    db.exec("delete from d_translator where f_id=:f_id");
    db[":f_dish"] = id;
    db.exec("delete from d_menu where f_dish=:f_dish");
    db[":f_id"] = id;
    db.exec("delete from d_dish where f_id=:f_id");
    fModel->removeRow(row);
    C5Message::info(tr("Deleted"));
}
