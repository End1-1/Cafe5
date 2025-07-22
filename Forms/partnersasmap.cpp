#include "partnersasmap.h"
#include "ui_partnersasmap.h"
#include "partnersasmap.h"
#include "c5lineedit.h"
#include "c5message.h"
#include "c5database.h"

PartnersAsMap::PartnersAsMap(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::PartnersAsMap)
{
    ui->setupUi(this);
    C5Database db;
    ui->tbl->setColumnWidths(5, 0, 100, 200, 200, 600);
    db.exec("select p.f_id, p.f_taxcode, p.f_name, p.f_taxname, p.f_address "
            "from c_partners p order by p.f_taxname ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    QMap<int, int> rowMap;
    while (db.nextRow()) {
        rowMap[db.getInt("f_id")] = row;
        ui->tbl->setInteger(row, 0, db.getInt("f_id"));
        ui->tbl->setString(row, 1, db.getString("f_taxcode"));
        ui->tbl->setString(row, 2, db.getString("f_name"));
        ui->tbl->setString(row, 3, db.getString("f_taxname"));
        ui->tbl->setString(row, 4, db.getString("f_address"));
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
    db[":f_table"] = "c_partners";
    db.exec("select * from as_convert where f_table=:f_table");
    while (db.nextRow()) {
        row = rowMap[db.getInt("f_tableid")];
        col = asList[db.getInt("f_asdbid")];
        ui->tbl->lineEdit(row, col)->setText(db.getString("f_ascode"));
    }
}

PartnersAsMap::~PartnersAsMap()
{
    delete ui;
}

QToolBar *PartnersAsMap::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void PartnersAsMap::saveDataChanges()
{
    C5Database db;
    db.exec("delete from as_convert where f_table='c_partners'");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        for (int c = ui->tbl->columnCount() - asList.count(); c < ui->tbl->columnCount(); c++) {
            if (!ui->tbl->lineEdit(i, c)->text().isEmpty()) {
                db[":f_table"] = "c_partners";
                db[":f_asdbid"] = asColId[c];
                db[":f_tableid"] = ui->tbl->getInteger(i, 0);
                db[":f_ascode"] = ui->tbl->lineEdit(i, c)->text();
                db.insert("as_convert", false);
            }
        }
    }
    C5Message::info(tr("Saved"));
}
