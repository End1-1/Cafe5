#include "cr5menureview.h"
#include "ce5editor.h"
#include "c5tablemodel.h"
#include "c5dishwidget.h"

CR5MenuReview::CR5MenuReview(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Review menu");
    fSqlQuery = "select d.f_id, mn.f_name as f_menuname, dp1.f_name as f_part1, dp2.f_name as f_part2, "
            "d.f_name as f_dishname, m.f_price, s.f_name as f_storename, m.f_print1, m.f_print2, d.f_color "
            "from d_menu m "
            "left join d_dish d on d.f_id=m.f_dish "
            "left join d_part2 dp2 on dp2.f_id=d.f_part "
            "left join d_part1 dp1 on dp1.f_id=dp2.f_part "
            "left join d_menu_names mn on mn.f_id=m.f_menu "
            "left join c_storages s on s.f_id=m.f_store "
            "order by mn.f_name, dp1.f_name, dp2.f_name ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_menuname"] = tr("Menu");
    fTranslation["f_part1"] = tr("Dept");
    fTranslation["f_part2"] = tr("Type");
    fTranslation["f_dishname"] = tr("Dish name");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_print1"] = tr("Print 1");
    fTranslation["f_print2"] = tr("Print 2");
    fTranslation["f_color"] = tr("Color");
    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(dblClick(int,int,QList<QVariant>)));
}

QToolBar *CR5MenuReview::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5MenuReview::refreshData()
{
    C5ReportWidget::refreshData();
    for (int i = 0; i < fModel->rowCount(); i++) {
        fModel->setRowColor(i, QColor::fromRgb(fModel->data(i, fModel->indexForColumnName("f_color"), Qt::EditRole).toInt()));
    }
}

void CR5MenuReview::dblClick(int row, int column, const QList<QVariant> &v)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (v.count() == 0) {
        return;
    }
    C5DishWidget *ep = new C5DishWidget(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(v.at(0).toInt());
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {

    }
    delete e;
}
