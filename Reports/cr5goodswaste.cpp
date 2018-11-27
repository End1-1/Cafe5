#include "cr5goodswaste.h"
#include "c5selector.h"
#include "c5tablemodel.h"
#include "ce5goodswaste.h"

CR5GoodsWaste::CR5GoodsWaste(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods auto waste");

    fSqlQuery = "select w.f_id, gg.f_name as f_goodsname, r.f_name as f_reasonname, w.f_waste \
                from c_goods_waste w \
                left join c_goods gg on gg.f_id=w.f_goods \
                left join a_reason r on r.f_id=w.f_reason ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_reasonname"] = tr("Reason");
    fTranslation["f_waste"] = tr("Waste %");
    fTranslation["f_goodsname"] = tr("Goods");;

    fEditor = new CE5GoodsWaste(dbParams);
}

QToolBar *CR5GoodsWaste::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbDelete
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5GoodsWaste::removeRow()
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove selected records")) != QDialog::Accepted) {
        return;
    }
    QSet<int> rowsTemp;
    foreach (QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }
    QList<int> rows = rowsTemp.toList();
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());
    C5Database db(fDBParams);
    foreach (int r, rows) {
        db[":f_id"] = fModel->data(r, 0, Qt::EditRole);
        db.exec("delete from c_goods_waste where f_id=:f_id");
        fModel->removeRow(r);
    }
}

