#include "cr5dishpart2.h"
#include "ce5dishpart2.h"
#include "c5tablemodel.h"
#include <QHeaderView>

CR5DishPart2::CR5DishPart2(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Types of dishes");
    fSqlQuery = "select t.f_id, p.f_name as part_name, t.f_name, t.f_adgCode, \
                t.f_queue, t.f_color \
                from d_part2 t \
                left join d_part1 p on p.f_id=t.f_part ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["part_name"] = tr("Dept name");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_queue"] = tr("Queue");
    fTranslation["f_color"] = tr("Color");
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
    }
    return fToolBar;
}

bool CR5DishPart2::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (C5Grid::on_tblView_doubleClicked(index)) {
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
