#include "cr5salefromstoretotal.h"
#include "c5tablemodel.h"
#include "cr5salefromstoretotalfilter.h"
#include "c5tablewidget.h"
#include "c5database.h"

CR5SaleFromStoreTotal::CR5SaleFromStoreTotal(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales from store total");
    fSimpleQuery = false;
    fFilterWidget = new CR5SaleFromStoreTotalFilter();
    fFilter = static_cast<CR5SaleFromStoreTotalFilter *>(fFilterWidget);
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
    fColumnNameIndex.clear();
    fTranslation.clear();
    fColumnsVisible.clear();
    fColumnNameIndex["f_group"] = 0;
    fColumnNameIndex["f_goodsid"] = 1;
    fColumnNameIndex["f_goodsname"] = 2;
    fColumnNameIndex["f_qtybegin"] = 3;
    fColumnNameIndex["f_sumbegin"] = 4;
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goodsid"] = tr("Goods") + "\r\n" +  tr("code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtybegin"] = tr("Initial") + "\r\n" + tr("Qty");
    fTranslation["f_sumbegin"] = tr("Initial") + "\r\n" + tr("Amount");
    C5Database db;
    QMap<int, int> typeMap1, typeMap2;
    db[":f_date1"] = fFilter->start();
    db[":f_date2"] = fFilter->end();
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_store"] = fFilter->store();
    db[":f_type"] = 1;
    db.exec("select distinct(r.f_name) as r_name, s.f_reason from a_store s "
            "inner join a_header h on h.f_id=s.f_document "
            "inner join a_reason r on r.f_id=s.f_reason "
            "where h.f_date between :f_date1 and :f_date2 "
            "and h.f_state=:f_state and s.f_store=:f_store "
            "and s.f_type=:f_type ");
    int nextCol = fColumnNameIndex.count();
    while (db.nextRow()) {
        typeMap1[db.getInt(1)] = nextCol;
        fColumnNameIndex[db.getString(0)] = nextCol++;
        fColumnNameIndex[db.getString(0) + "," + tr("Amount")] = nextCol++;
        fTranslation[db.getString(0)] = db.getString(0) + "\r\n" + tr("Qty");
        fTranslation[db.getString(0) + "," + tr("Amount")] = db.getString(0) + "\r\n" + tr("Amount");
    }
    db[":f_date1"] = fFilter->start();
    db[":f_date2"] = fFilter->end();
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_store"] = fFilter->store();
    db[":f_type"] = -1;
    db.exec("select distinct(r.f_name) as r_name, s.f_reason from a_store s "
            "inner join a_header h on h.f_id=s.f_document "
            "inner join a_reason r on r.f_id=s.f_reason "
            "where h.f_date between :f_date1 and :f_date2 "
            "and h.f_state=:f_state and s.f_store=:f_store "
            "and s.f_type=:f_type ");
    while (db.nextRow()) {
        typeMap2[db.getInt(1)] = nextCol;
        fColumnNameIndex[db.getString(0)] = nextCol++;
        fColumnNameIndex[db.getString(0) + "," + tr("Amount")] = nextCol++;
        fTranslation[db.getString(0)] = db.getString(0) + "\r\n" + tr("Qty");
        fTranslation[db.getString(0) + "," + tr("Amount")] = db.getString(0) + "\r\n" + tr("Amount");
    }
    fColumnNameIndex["f_final"] = nextCol++;
    fColumnNameIndex["f_finalamount"] = nextCol++;
    fTranslation["f_final"] = tr("Final, qty");
    fTranslation["f_finalamount"] = tr("Final amount");
    std::vector<QJsonArray > &rows = fModel->fRawData;
    rows.clear();
    //names
    db.exec("select g.f_id, g.f_name, gr.f_name as f_groupname from c_goods g inner join c_groups gr on gr.f_id=g.f_group order by f_name");
    int row = 0;
    QMap<int, int> goodsRowMap;
    while (db.nextRow()) {
        QJsonArray emptyRow;
        for (int i = 0; i < 14; i++) {
            emptyRow << QJsonValue();
        }
        emptyRow[0] = db.getString("f_groupname");
        emptyRow[1] = db.getInt("f_id");
        emptyRow[2] = db.getString("f_name");
        rows.push_back(emptyRow);
        goodsRowMap[db.getInt(0)] = row++;
    }
    QString query = QString("select s.f_goods, gg.f_name as f_groupname, g.f_name as f_goodsname, u.f_name as f_unitname, "
                            "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
                            "from a_store s "
                            "inner join a_header d on d.f_id=s.f_document "
                            "inner join c_goods g on g.f_id=s.f_goods "
                            "inner join c_groups gg on gg.f_id=g.f_group "
                            "inner join c_units u on u.f_id=g.f_unit "
                            "where s.f_store=%1 and d.f_date<'%2'"
                            "group by 1, 2, 3, 4 "
                            "having sum(s.f_qty*s.f_type) > 0.001 ")
                    .arg(fFilter->store())
                    .arg(fFilter->start().toString(FORMAT_DATE_TO_STR_MYSQL));
    db.exec(query);
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        rows[row][3] = db.getDouble("f_qty");
        rows[row][4] = db.getDouble("f_amount");
    }
    db[":f_date1"] = fFilter->start();
    db[":f_date2"] = fFilter->end();
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_store"] = fFilter->store();
    db.exec("select s.f_goods, s.f_type, s.f_reason, "
            "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_groups gg on gg.f_id=g.f_group "
            "inner join c_units u on u.f_id=g.f_unit "
            "where s.f_store=:f_store and d.f_date between :f_date1 and :f_date2 "
            "group by 1, 2, 3 ");
    while (db.nextRow()) {
        int row = goodsRowMap[db.getInt("f_goods")];
        int col = 0;
        if (db.getInt(1) == 1) {
            col = typeMap1[db.getInt(2)];
        } else {
            col = typeMap2[db.getInt(2)];
        }
        rows[row][col] = db.getDouble("f_qty");
        rows[row][col + 1] = db.getDouble("f_amount");
    }
    for (int i = 0; i < rows.size(); i++) {
    }
    for (int i = rows.size() - 1; i > -1; i--) {
        bool remove = true;
        for (int j = 3; j < nextCol - 3; j += 2) {
            rows[i][nextCol - 2] = rows[i][nextCol - 2].toDouble() + rows[i][j].toDouble();
            rows[i][nextCol - 1] = rows[i][nextCol - 1].toDouble() + rows[i][j + 1].toDouble();
            if (rows[i][j].toDouble() > -0.0001 && rows[i][j].toDouble() < 0.0001) {
                remove = false;
            }
        }
        if (remove) {
            rows.erase(rows.begin() + i);
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    for (QHash<QString, int>::const_iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
        fColumnsVisible[it.key()] = true;
    }
    restoreColumnsWidths();
    QStringList totalList;
    totalList.append(QString::number(rows.size()));
    fTableTotal->setVerticalHeaderLabels(totalList);
    fTableTotal->setVisible(true);
    for (int i = 3; i < nextCol; i++) {
        double total = 0;
        for (int j = 0; j < rows.size(); j++) {
            total += rows[j][i].toDouble();
        }
        fTableTotal->setData(0, i, total);
    }
    for (int j = 3; j < nextCol; j++) {
        if (j % 2 != 0) {
            fTableView->setColumnHidden(j, !fFilter->showQty());
            fTableTotal->setColumnHidden(j, !fFilter->showQty());
            if (fFilter->showQty() && fTableTotal->columnWidth(j) == 0) {
                fTableView->setColumnWidth(j, 80);
                fTableTotal->setColumnWidth(j, 80);
            }
        } else {
            fTableView->setColumnHidden(j, !fFilter->showAmount());
            fTableTotal->setColumnHidden(j, !fFilter->showAmount());
            if (fFilter->showAmount() && fTableTotal->columnWidth(j) == 0) {
                fTableView->setColumnWidth(j, 80);
                fTableTotal->setColumnWidth(j, 80);
            }
        }
    }
}

void CR5SaleFromStoreTotal::refreshData()
{
    buildQuery();
}
