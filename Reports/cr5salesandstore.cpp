#include "cr5salesandstore.h"
#include "c5tablemodel.h"
#include "cr5saleandstorefilter.h"

CR5SalesEffectiveness::CR5SalesEffectiveness(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Effectiveness of sales");
    fIcon = ":/effectiveness.png";
    fFilterWidget = new CR5SaleAndStoreFilter(fDBParams);
    fFilter = static_cast<CR5SaleAndStoreFilter*>(fFilterWidget);
}

QToolBar *CR5SalesEffectiveness::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
            btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel;
            fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5SalesEffectiveness::buildQuery()
{
    fColumnsSum.clear();
    fTranslation.clear();
    fColumnNameIndex.clear();
    fColumnsFields.clear();

    switch (fFilter->display()) {
    case CR5SaleAndStoreFilter::dGoods:
        rep1();
        break;
    case CR5SaleAndStoreFilter::dGroups:
        rep2();
        break;
    }
}

void CR5SalesEffectiveness::refreshData()
{
    buildQuery();
}

void CR5SalesEffectiveness::rep1()
{
    fColumnNameIndex["f_goodsid"] = 0;
    fColumnNameIndex["f_groupname"] = 1;
    fColumnNameIndex["f_goodsname"] = 2;
    fColumnNameIndex["f_scancode"] = 3;
    fColumnNameIndex["f_class1"] = 4;
    fColumnNameIndex["f_class2"] = 5;
    fColumnNameIndex["f_class3"] = 6;
    fColumnNameIndex["f_class4"] = 7;
    fColumnNameIndex["f_startqty"] = 8;
    fColumnNameIndex["f_inputqty"] = 9;
    fColumnNameIndex["f_outputqty"] = 10;
    fColumnNameIndex["f_output"] = 11;
    fColumnNameIndex["f_finalqty"] = 12;
    fColumnNameIndex["f_effectivity"] = 13;
    fColumnNameIndex["f_storedelta"] = 14;

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_class1"] = tr("Class 1");
    fTranslation["f_class2"] = tr("Class 2");
    fTranslation["f_class3"] = tr("Class 3");
    fTranslation["f_class4"] = tr("Class 4");
    fTranslation["f_startqty"] = tr("Start qty");
    fTranslation["f_inputqty"] = tr("Input qty");
    fTranslation["f_outputqty"] = tr("Sales qty");
    fTranslation["f_output"] = tr("Output");
    fTranslation["f_finalqty"] = tr("Final qty");
    fTranslation["f_effectivity"] = tr("Effectivity");
    fTranslation["f_storedelta"] = tr("Store delta");

    QList<QList<QVariant> > &rows = fModel->fRawData;
    rows.clear();
    QHash<int, int> goodsRowMap;
    C5Database db(fDBParams);

    QHash<int, QString> storeList;
    QString query = "select f_id, f_name from c_storages where f_id>0 ";
    if (!fFilter->store().isEmpty()) {
        query += QString(" and f_id in (%1) ").arg(fFilter->store());
    }
    db.exec(query);
    while (db.nextRow()){
        storeList[db.getInt("f_id")] = db.getString("f_name");
    }

    query = "select g.f_id as f_goodsid, gr.f_name as f_groupname,  g.f_name as f_goodsname, g.f_scancode, "
                    "gc1.f_name as f_class1, gc2.f_name as f_class2, gc3.f_name as f_class3, gc4.f_name as f_class4 "
                    "from c_goods g "
                    "left join c_groups gr on gr.f_id=g.f_group "
                    "left join c_goods_classes gc1 on gc1.f_id=g.f_group1 "
                    "left join c_goods_classes gc2 on gc2.f_id=g.f_group2 "
                    "left join c_goods_classes gc3 on gc3.f_id=g.f_group3 "
                    "left join c_goods_classes gc4 on gc4.f_id=g.f_group4 "
                    "where g.f_id>0 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    db.exec(query);
    int storeIdx = db.columnCount();
    while (db.nextRow()) {
        QList<QVariant> row;
        for (int i = 0, c = db.columnCount(); i < c; i++) {
            row.append(db.getValue(i));
        }
        //store
        row.append(0);
        //input
        row.append(0);
        //sales
        row.append(0);
        //output
        row.append(0);
        //final
        row.append(0);
        //effectivity
        row.append(0);
        //store delta
        row.append(0);

        rows.append(row);
        goodsRowMap[db.getInt("f_goodsid")] = rows.count() - 1;
    }
    //START QTY
    query = "select s.f_goods as f_code, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total "
            "from a_store s "
            "inner join a_header h on h.f_id=s.f_document  ";
    if (!fFilter->goods().isEmpty() || !fFilter->goodsGroup().isEmpty()) {
        query += "inner join c_goods g on g.f_id=s.f_goods ";
    }
    query += "where h.f_date<:f_date and h.f_state=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date"] = fFilter->date1();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx;
        int r = goodsRowMap[db.getInt("f_goods")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //INPUT QTY
    query = "select s.f_goods ,sum(s.f_qty) as f_qty "
            "from a_store s "
            "left join a_header a on a.f_id=s.f_document  ";
    if (!fFilter->goods().isEmpty() || !fFilter->goodsGroup().isEmpty()) {
        query += "inner join c_goods g on g.f_id=s.f_goods ";
    }
    query += "where  a.f_date between :f_date1 and :f_date2 and s.f_type=1  ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 1;
        int r = goodsRowMap[db.getInt("f_goods")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //SALES QTY
    query = "select og.f_goods,sum(og.f_qty) as f_qty "
            "from o_goods og "
            "left join o_header oh on oh.f_id=og.f_header ";
    if (!fFilter->goods().isEmpty() || !fFilter->goodsGroup().isEmpty()) {
        query += "inner join c_goods g on g.f_id=og.f_goods ";
    }
    query += "where oh.f_datecash between :f_date1 and :f_date2 and og.f_sign=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and og.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 2;
        int r = goodsRowMap[db.getInt("f_goods")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //OTHER OUTPUT
    query = "select s.f_goods ,sum(s.f_qty) as f_qty "
            "from a_store s "
            "left join a_header a on a.f_id=s.f_document  ";
    if (!fFilter->goods().isEmpty() || !fFilter->goodsGroup().isEmpty()) {
        query += "inner join c_goods g on g.f_id=s.f_goods ";
    }
    query += "where  a.f_date between :f_date1 and :f_date2 and s.f_type=-1 and s.f_reason <>4 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 3;
        int r = goodsRowMap[db.getInt("f_goods")];
        rows[r][c] = db.getDouble("f_qty");
    }

    //FINAL QTY
    query = "select s.f_goods as f_code, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total "
            "from a_store s "
            "inner join a_header h on h.f_id=s.f_document  ";
    if (!fFilter->goods().isEmpty() || !fFilter->goodsGroup().isEmpty()) {
        query += "inner join c_goods g on g.f_id=s.f_goods ";
    }
    query += "where h.f_date<=:f_date and h.f_state=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->goods().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goods());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 4;
        int r = goodsRowMap[db.getInt("f_goods")];
        rows[r][c] = db.getDouble("f_qty");
    }
    for (int i = rows.count() - 1; i > -1; i--) {
        if (rows[i][storeIdx].toDouble() < 0.0001
                && rows[i][storeIdx + 1].toDouble() < 0.0001
                && rows[i][storeIdx + 2].toDouble() < 0.0001
                && rows[i][storeIdx + 3].toDouble() < 0.0001) {
            rows.removeAt(i);
        }
    }
    for (int i = rows.count() - 1; i > -1; i--) {
        if ((rows[i][storeIdx].toDouble() + rows[i][storeIdx + 1].toDouble()) > 0.0001) {
            rows[i][storeIdx + 5] = (rows[i][storeIdx + 2].toDouble() * 100) / (rows[i][storeIdx].toDouble() + rows[i][storeIdx + 1].toDouble());
        }
        if (rows[i][storeIdx].toDouble() > 0.0001) {
            rows[i][storeIdx + 6] = ((rows[i][storeIdx + 4].toDouble() * 100) / rows[i][storeIdx].toDouble()) - 100;
        } else {
            rows[i][storeIdx + 6] = 100.0;
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    fColumnsSum.clear();
    fColumnsSum.append("f_startqty");
    fColumnsSum.append("f_inputqty");
    fColumnsSum.append("f_outputqty");
    fColumnsSum.append("f_output");
    fColumnsSum.append("f_finalqty");
    sumColumnsData();
    restoreColumnsWidths();
}

void CR5SalesEffectiveness::rep2()
{
    fColumnNameIndex["f_groupid"] = 0;
    fColumnNameIndex["f_groupname"] = 1;
    fColumnNameIndex["f_startqty"] = 2;
    fColumnNameIndex["f_inputqty"] = 3;
    fColumnNameIndex["f_outputqty"] = 4;
    fColumnNameIndex["f_output"] = 5;
    fColumnNameIndex["f_finalqty"] = 6;
    fColumnNameIndex["f_effectivity"] = 7;
    fColumnNameIndex["f_storedelta"] = 8;

    fTranslation["f_groupid"] = tr("Group code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_startqty"] = tr("Start qty");
    fTranslation["f_inputqty"] = tr("Input qty");
    fTranslation["f_outputqty"] = tr("Sale qty");
    fTranslation["f_output"] = tr("Output qty");
    fTranslation["f_finalqty"] = tr("Final qty");
    fTranslation["f_effectivity"] = tr("Effectivity");
    fTranslation["f_storedelta"] = tr("Store delta");

    QList<QList<QVariant> > &rows = fModel->fRawData;
    rows.clear();
    QHash<int, int> goodsRowMap;
    C5Database db(fDBParams);

    QString query = "select g.f_id as f_groupid, g.f_name as f_groupname "
                    "from c_groups g "
                    "where g.f_id>0 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_id in (%1) ").arg(fFilter->goodsGroup());
    }
    db.exec(query);
    int storeIdx = db.columnCount();
    while (db.nextRow()) {
        QList<QVariant> row;
        for (int i = 0, c = db.columnCount(); i < c; i++) {
            row.append(db.getValue(i));
        }
        //store
        row.append(0);
        //input
        row.append(0);
        //sales
        row.append(0);
        //output
        row.append(0);
        //final
        row.append(0);
        //effectivity
        row.append(0);
        //store delta
        row.append(0);

        rows.append(row);
        goodsRowMap[db.getInt("f_groupid")] = rows.count() - 1;
    }
    //START QTY
    query = "select g.f_group, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total "
            "from a_store s "
            "inner join a_header h on h.f_id=s.f_document  "
            "inner join c_goods g on g.f_id=s.f_goods "
            "where h.f_date<:f_date and h.f_state=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date"] = fFilter->date1();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx;
        int r = goodsRowMap[db.getInt("f_group")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //INPUT QTY
    query = "select g.f_group ,sum(s.f_qty) as f_qty "
            "from a_store s "
            "left join a_header a on a.f_id=s.f_document  "
            "inner join c_goods g on g.f_id=s.f_goods "
            "where  a.f_date between :f_date1 and :f_date2 and s.f_type=1  ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 1;
        int r = goodsRowMap[db.getInt("f_group")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //SALES QTY
    query = "select g.f_group,sum(og.f_qty) as f_qty "
            "from o_goods og "
            "left join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "where oh.f_datecash between :f_date1 and :f_date2 and og.f_sign=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and og.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 2;
        int r = goodsRowMap[db.getInt("f_group")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //OUTPUT QTY
    query = "select g.f_group ,sum(s.f_qty) as f_qty "
            "from a_store s "
            "left join a_header a on a.f_id=s.f_document  "
            "inner join c_goods g on g.f_id=s.f_goods "
            "where  a.f_date between :f_date1 and :f_date2 and s.f_type=-1 and s.f_reason<>4 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 3;
        int r = goodsRowMap[db.getInt("f_group")];
        rows[r][c] = db.getDouble("f_qty");
    }
    //FINAL QTY
    query = "select g.f_group, sum(s.f_qty*s.f_type) as f_qty,sum(s.f_total*s.f_type) as f_total "
            "from a_store s "
            "inner join a_header h on h.f_id=s.f_document  "
            "inner join c_goods g on g.f_id=s.f_goods "
            "where h.f_date<=:f_date and h.f_state=1 ";
    if (!fFilter->goodsGroup().isEmpty()) {
        query += QString(" and g.f_group in (%1) ").arg(fFilter->goodsGroup());
    }
    if (!fFilter->store().isEmpty()) {
        query += QString(" and s.f_store in (%1) ").arg(fFilter->store());
    }
    query += "group by 1 ";
    db[":f_date"] = fFilter->date2();
    db.exec(query);
    while (db.nextRow()) {
        int c = storeIdx + 4;
        int r = goodsRowMap[db.getInt("f_group")];
        rows[r][c] = db.getDouble("f_qty");
    }
    for (int i = rows.count() - 1; i > -1; i--) {
        if (rows[i][storeIdx].toDouble() < 0.0001
                && rows[i][storeIdx + 1].toDouble() < 0.0001
                && rows[i][storeIdx + 2].toDouble() < 0.0001
                && rows[i][storeIdx + 4].toDouble() < 0.0001) {
            rows.removeAt(i);
        }
    }
    for (int i = rows.count() - 1; i > -1; i--) {
        if ((rows[i][storeIdx].toDouble() + rows[i][storeIdx + 1].toDouble()) > 0.0001) {
            rows[i][storeIdx + 5] = (rows[i][storeIdx + 2].toDouble() * 100) / (rows[i][storeIdx].toDouble() + rows[i][storeIdx + 1].toDouble());
        }
        if (rows[i][storeIdx].toDouble() > 0.0001) {
            rows[i][storeIdx + 6] = ((rows[i][storeIdx + 4].toDouble() * 100) / rows[i][storeIdx].toDouble()) - 100;
        } else {
            rows[i][storeIdx + 6] = 100.0;
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    fColumnsSum.clear();
    fColumnsSum.append("f_startqty");
    fColumnsSum.append("f_inputqty");
    fColumnsSum.append("f_outputqty");
    fColumnsSum.append("f_output");
    fColumnsSum.append("f_finalqty");
    restoreColumnsWidths();
    sumColumnsData();
}
