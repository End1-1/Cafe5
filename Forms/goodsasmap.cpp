#include "goodsasmap.h"
#include "ui_goodsasmap.h"
#include "c5lineedit.h"
#include "c5message.h"

GoodsAsMap::GoodsAsMap(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::GoodsAsMap)
{
    ui->setupUi(this);
    C5Database db(fDBParams);
    ui->tbl->setColumnWidths(3, 0, 150, 400);
    db.exec("select g.f_id, gr.f_name as f_group, g.f_name "
            "from c_goods g "
            "left join c_groups gr on gr.f_id=g.f_group "
            "where g.f_enabled=1 "
            "order by 2, 3 ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    QMap<int, int> rowMap;
    while (db.nextRow()) {
        rowMap[db.getInt("f_id")] = row;
        ui->tbl->setInteger(row, 0, db.getInt("f_id"));
        ui->tbl->setString(row, 1, db.getString("f_group"));
        ui->tbl->setString(row, 2, db.getString("f_name"));
        row++;
    }
    db.exec("select * from as_list");
    int col = ui->tbl->columnCount();
    ui->tbl->setColumnCount(col + db.rowCount());
    while (db.nextRow()) {
        ui->tbl->setHorizontalHeaderItem(col, new QTableWidgetItem(db.getString("f_name")));
        asList[db.getInt("f_id")] = col;
        asColId[col] = db.getInt("f_id");
        col++;
    }
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        for (int c = ui->tbl->columnCount() - asList.count(); c < ui->tbl->columnCount(); c++) {
            ui->tbl->createLineEdit(r, c);
            ui->tbl->setColumnWidth(c, 80);
        }
    }
    db[":f_table"] = "c_goods";
    db.exec("select * from as_convert where f_table=:f_table");
    while (db.nextRow()) {
        row = rowMap[db.getInt("f_tableid")];
        col = asList[db.getInt("f_asdbid")];
        ui->tbl->lineEdit(row, col)->setText(db.getString("f_ascode"));
    }
}

GoodsAsMap::~GoodsAsMap()
{
    delete ui;
}

QToolBar *GoodsAsMap::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void GoodsAsMap::saveDataChanges()
{
    C5Database db(fDBParams);
    db.exec("delete from as_convert where f_table='c_goods'");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        for (int c = ui->tbl->columnCount() - asList.count(); c < ui->tbl->columnCount(); c++) {
            if (!ui->tbl->lineEdit(i, c)->text().isEmpty()) {
                db[":f_table"] = "c_goods";
                db[":f_asdbid"] = asColId[c];
                db[":f_tableid"] = ui->tbl->getInteger(i, 0);
                db[":f_ascode"] = ui->tbl->lineEdit(i, c)->text();
                db.insert("as_convert", false);
            }
        }
    }
    C5Message::info(tr("Saved"));
}
