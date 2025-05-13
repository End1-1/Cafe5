#include "cr5menureview.h"
#include "c5database.h"
#include "c5tablemodel.h"
#include "c5dishwidget.h"
#include "cr5menureviewfilter.h"
#include "c5dishgroupaction.h"

CR5MenuReview::CR5MenuReview(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Review menu");
    fSqlQuery =
        "select  d.f_id, m.f_id as f_mid, md.f_name as f_statename, mn.f_name as f_menuname, dp1.f_name as f_part1, dp2.f_name as f_part2, "
        "d.f_name as f_dishname, m.f_price, d.f_cost, d.f_recipeqty, s.f_name as f_storename, m.f_print1, m.f_print2, d.f_color "
        "from d_menu m "
        "left join d_dish_state md on md.f_id=m.f_state "
        "left join d_dish d on d.f_id=m.f_dish "
        "left join d_part2 dp2 on dp2.f_id=d.f_part "
        "left join d_part1 dp1 on dp1.f_id=dp2.f_part "
        "left join d_menu_names mn on mn.f_id=m.f_menu "
        "left join c_storages s on s.f_id=m.f_store ";
    fOrderCondition = "order by mn.f_name, dp1.f_name, dp2.f_name ";
    fTranslation["f_mid"] = tr("Id");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_menuname"] = tr("Menu");
    fTranslation["f_part1"] = tr("Dept");
    fTranslation["f_part2"] = tr("Type");
    fTranslation["f_dishname"] = tr("Dish name");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_cost"] = tr("Selfcost");
    fTranslation["f_recipeqty"] = tr("Recipe weight");
    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_print1"] = tr("Print 1");
    fTranslation["f_print2"] = tr("Print 2");
    fTranslation["f_color"] = tr("Color");
    fEditor = new C5DishWidget(dbParams);
    restoreColumnsVisibility();
    fFilterWidget = new CR5MenuReviewFilter(dbParams);
}

QToolBar *CR5MenuReview::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/hammer.png"), tr("Set group\naction"), this, SLOT(groupAction()));
    }
    return fToolBar;
}

void CR5MenuReview::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["f_mid"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_mid"], 0);
    }
}

void CR5MenuReview::refreshData()
{
    C5ReportWidget::refreshData();
    for (int i = 0; i < fModel->rowCount(); i++) {
        fModel->setRowColor(i, QColor::fromRgb(fModel->data(i, fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
}

bool CR5MenuReview::tblDoubleClicked(int row, int column, const QJsonArray &v)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (v.count() == 0) {
        return true;
    }
    C5DishWidget *ep = new C5DishWidget(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    int col = fModel->indexForColumnName("f_id");
    ep->setId(v.at(col).toInt());
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
    }
    delete e;
    return true;
}

void CR5MenuReview::groupAction()
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return;
    }
    QSet<int> rows;
    for (int i = 0; i < ml.count(); i++) {
        rows.insert(ml.at(i).row());
    }
    int col = fModel->indexForColumnName("f_mid");
    C5DishGroupAction *da = new C5DishGroupAction(fDBParams);
    if (da->exec() == QDialog::Accepted) {
        C5Database db(fDBParams);
        bool work = false;
        QString id;
        if (da->setStore(id)) {
            foreach (int row, rows) {
                db[":f_store"] = id;
                db.update("d_menu", where_id(fModel->data(row, col, Qt::EditRole).toString()));
            }
            work = true;
        }
        if (da->setState(id)) {
            foreach (int row, rows) {
                db[":f_state"] = id;
                db.update("d_menu", where_id(fModel->data(row, col, Qt::EditRole).toString()));
            }
            work = true;
        }
        if (work) {
            C5Message::info(tr("Done"));
        }
    }
    delete da;
}
