#include "cr5salefromstoretotal.h"
#include "c5tablemodel.h"
#include "cr5salefromstoretotalfilter.h"

CR5SaleFromStoreTotal::CR5SaleFromStoreTotal(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales from store total");
    fSimpleQuery = false;

    fColumnNameIndex["f_goodsid"] = 0;
    fColumnNameIndex["f_goodsname"] = 1;
    fColumnNameIndex["f_qtybegin"] = 2;
    fColumnNameIndex["f_sumbegin"] = 3;
//    fColumnNameIndex["f_qtyin"] = 4;
//    fColumnNameIndex["f_sumin"] = 5;
//    fColumnNameIndex["f_qtyout"] = 6;
//    fColumnNameIndex["f_sumout"] = 7;
//    fColumnNameIndex["f_qtymust"] = 8;
//    fColumnNameIndex["f_summust"] = 9;
//    fColumnNameIndex["f_qtyinv"] = 10;
//    fColumnNameIndex["f_suminv"] = 11;
//    fColumnNameIndex["f_qtydif"] = 12;
//    fColumnNameIndex["f_sumdif"] = 13;

//    fColumnsSum << "f_qtybegin"
//                << "f_sumbegin"
//                << "f_qtyin"
//                << "f_sumin"
//                << "f_qtyout"
//                << "f_sumout"
//                << "f_qtymust"
//                << "f_summust"
//                << "f_qtyinv"
//                << "f_suminv"
//                << "f_qtydif"
//                << "f_sumdif";

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtybegin"] = tr("Qty, begin");
    fTranslation["f_sumbegin"] = tr("Sum, begin");
//    fTranslation["f_qtyin"] = tr("Qty, in");
//    fTranslation["f_sumin"] = tr("Sum, in");
//    fTranslation["f_qtyout"] = tr("Qty, out");
//    fTranslation["f_sumout"] = tr("Sum, out");
//    fTranslation["f_qtymust"] = tr("Qty, must");
//    fTranslation["f_summust"] = tr("Sum, must");
//    fTranslation["f_qtyinv"] = tr("Qty, inv");
//    fTranslation["f_suminv"] = tr("Sum, inv");
//    fTranslation["f_qtydif"] = tr("Qty, dif");
//    fTranslation["f_sumdif"] = tr("Sum, dif");
}

QToolBar *CR5SaleFromStoreTotal::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5SaleFromStoreTotal::buildQuery()
{
    QList<QList<QVariant> > &rows = fModel->fRawData;
    rows.clear();
    C5Database db(fDBParams);

    //names
    db.exec("select f_id, f_name from c_goods order by f_name");
    int row = 0;
    QMap<int, int> goodsRowMap;
    while (db.nextRow()) {
        QList<QVariant> emptyRow;
        for (int i = 0; i < 14; i++) {
            emptyRow << QVariant();
        }
        emptyRow[0] = db.getInt("f_id");
        emptyRow[1] = db.getString("f_name");
        rows << emptyRow;
        goodsRowMap[db.getInt(0)] = row++;
    }

//    db[":f_store"] = fFilter->store();
//    db[":f_date"] = fFilter->dateStart();
    db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from a_store s "
              "inner join a_header d on d.f_id=s.f_document "
              "where s.f_store=:f_store and d.f_date<:f_date "
              "group by 1  "
              "having sum(s.f_qty*s.f_type) > 0.001 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][2] = db.getDouble("f_qty");
        rows[row][3] = db.getDouble("f_amount");
    }

    fModel->setExternalData(fColumnNameIndex, fTranslation);
}

void CR5SaleFromStoreTotal::refreshData()
{
    buildQuery();
}
