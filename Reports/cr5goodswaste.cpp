#include "cr5goodswaste.h"
#include "c5selector.h"
#include "c5tablemodel.h"

CR5GoodsWaste::CR5GoodsWaste(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods auto waste");

    fSqlQuery = "select w.f_id, g.f_name as f_groupname, gg.f_name, w.f_waste \
                from c_goods_waste w \
                left join c_goods gg on gg.f_id=w.f_goods \
                left join c_groups g on g.f_id=gg.f_group ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_waste"] = tr("Waste %");
    fTranslation["f_name"] = tr("Name");;

    C5TextDelegate *te = new C5TextDelegate(fTableView);
    te->setValidator(new QDoubleValidator(0, 9, 3));
    fTableView->setItemDelegateForColumn(3, te);

    QList<int> colsForUpdate;
    colsForUpdate << 3;
    setTableForUpdate("c_goods_waste", colsForUpdate);

    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(dblClick(int,int,QList<QVariant>)));
}

QToolBar *CR5GoodsWaste::toolBar()
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

void CR5GoodsWaste::dblClick(int row, int column, const QList<QVariant> &values)
{
    QList<QVariant> v;
    if (column < 3) {
        if (!C5Selector::getValue(fDBParams, cache_goods, v)) {
            return;
        }
        fModel->setData(row, 1, v.at(1));
        fModel->setData(row, 2, v.at(2));
        fModel->setRowToUpdate(row, "f_goods", v.at(0));
    }
    qDebug() << row << column << values;
}
