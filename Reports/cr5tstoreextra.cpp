#include "cr5tstoreextra.h"
#include "c5tablemodel.h"
#include "cr5tstoreextrafilter.h"
#include "c5gridgilter.h"

CR5TStoreExtra::CR5TStoreExtra(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/documents.png";
    fLabel = tr("T-account, extra");
    fSimpleQuery = false;

    fColumnNameIndex["f_goodsid"] = 0;
    fColumnNameIndex["f_goodsname"] = 1;
    fColumnNameIndex["f_qtybegin"] = 2;
    fColumnNameIndex["f_sumbegin"] = 3;
    fColumnNameIndex["f_qtyin"] = 4;
    fColumnNameIndex["f_sumin"] = 5;
    fColumnNameIndex["f_qtyout"] = 6;
    fColumnNameIndex["f_sumout"] = 7;
    fColumnNameIndex["f_qtymust"] = 8;
    fColumnNameIndex["f_summust"] = 9;
    fColumnNameIndex["f_qtyinv"] = 10;
    fColumnNameIndex["f_suminv"] = 11;
    fColumnNameIndex["f_qtydif"] = 12;
    fColumnNameIndex["f_sumdif"] = 13;

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtybegin"] = tr("Qty, begin");
    fTranslation["f_sumbegin"] = tr("Sum, begin");
    fTranslation["f_qtyin"] = tr("Qty, in");
    fTranslation["f_sumin"] = tr("Sum, in");
    fTranslation["f_qtyout"] = tr("Qty, out");
    fTranslation["f_sumout"] = tr("Sum, out");
    fTranslation["f_qtymust"] = tr("Qty, must");
    fTranslation["f_summust"] = tr("Sum, must");
    fTranslation["f_qtyinv"] = tr("Qty, inv");
    fTranslation["f_suminv"] = tr("Sum, inv");
    fTranslation["f_qtydif"] = tr("Qty, dif");
    fTranslation["f_sumdif"] = tr("Sum, dif");

    fFilterWidget = new CR5TStoreExtraFilter(dbParams);
}

QToolBar *CR5TStoreExtra::toolBar()
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

void CR5TStoreExtra::buildQuery()
{
    CR5TStoreExtraFilter *f = static_cast<CR5TStoreExtraFilter*>(fFilterWidget);
    if (f->store() ==  0) {
        if (!C5GridGilter::filter(f, fWhereCondition, fColumnsVisible, fTranslation)) {
            return;
        }
    }
    if (f->store() == 0) {
        C5Message::info(tr("Store must be defined"));
        return;
    }
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

    //begin
    db[":f_store"] = f->store();
    db[":f_date"] = f->dateStart();
    db.exec("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from a_store s "
              "inner join a_header d on d.f_id=s.f_document "
              "where s.f_store=:f_store and d.f_date<=:f_date "
              "group by 1, 2 "
              "having sum(s.f_qty*s.f_type) > 0.001 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][2] = db.getDouble("f_qty");
        rows[row][3] = db.getDouble("f_amount");
    }

    //inputs
    db[":f_store"] = f->store();
    db[":f_type"] = 1;
    db[":f_date1"] = f->dateStart();
    db[":f_date2"] = f->dateEnd();
    db.exec("select s.f_goods, sum(s.f_qty) as f_qty, sum(s.f_total) as f_amount "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_type=:f_type and s.f_store=:f_store and d.f_date between :f_date1 and :f_date2 "
            "group by 1 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][4] = db.getDouble("f_qty");
        rows[row][5] = db.getDouble("f_amount");
    }

    //outputs
    db[":f_store"] = f->store();
    db[":f_type"] = -1;
    db[":f_date1"] = f->dateStart();
    db[":f_date2"] = f->dateEnd();
    db.exec("select s.f_goods, sum(s.f_qty) as f_qty, sum(s.f_total) as f_amount "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_type=:f_type and s.f_store=:f_store and d.f_date between :f_date1 and :f_date2 "
            "group by 1 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][6] = db.getDouble("f_qty");
        rows[row][7] = db.getDouble("f_amount");
    }

    //fact in the end, inventory
    db[":f_store"] = f->store();
    db[":f_date"] = f->dateEnd();
    db.exec("select s.f_goods, sum(s.f_qty) as f_qty, 0 as f_amount "
            "from a_store_inventory s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_store=:f_store and d.f_date = :f_date "
            "group by 1 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][10] = db.getDouble("f_qty");
        rows[row][11] = db.getDouble("f_amount");
    }

    //finaly, count
    for (int i = 0; i < rows.count(); i++) {
        rows[i][8] = rows[i][2].toDouble() + rows[i][4].toDouble() - rows[i][6].toDouble();
        rows[i][9] = rows[i][3].toDouble() + rows[i][5].toDouble() - rows[i][7].toDouble();
        double qty = rows[i][2].toDouble() + rows[i][4].toDouble();
        if (qty > 0.0001 && rows[i][10].toDouble() > 0.0001) {
            rows[i][11] = (rows[i][3].toDouble() + rows[i][5].toDouble()) / qty;
        }
        rows[i][12] = rows[i][10].toDouble() - rows[i][8].toDouble();
        rows[i][13] = rows[i][11].toDouble() - rows[i][9].toDouble();
    }

    //clear unused
    for (int i = rows.count() - 1; i > -1; i--) {
        if (rows[i][2].toDouble() < 0.001
                && rows[i][4].toDouble() < 0.001
                && rows[i][6].toDouble() < 0.001
                && rows[i][10].toDouble() < 0.001) {
            rows.removeAt(i);
        }
    }

    fModel->setExternalData(fColumnNameIndex, fTranslation);
    restoreColumnsWidths();
}

void CR5TStoreExtra::refreshData()
{
    buildQuery();
}
