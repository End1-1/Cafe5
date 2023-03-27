#include "c5route.h"
#include "ui_c5route.h"
#include "c5message.h"
#include "c5checkbox.h"

C5Route::C5Route(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5Route)
{
    ui->setupUi(this);
    fLabel = tr("Route");
    fIcon = ":/route.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 300, 300, 100, 40, 40, 40, 40, 40, 40, 40);
    C5Database db(dbParams);
    db.exec("select f_id, f_address, f_taxname, f_taxcode from c_partners order by 2");
    QMap<int, int> rowParner;
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setString(r, i + 1, db.getString(i));
        }
        for (int i = 5; i < ui->tbl->columnCount(); i++) {
            ui->tbl->createCheckbox(r, i);
        }
        rowParner[db.getInt(0)] = r;
    }
    db.exec("select * from o_route");
    while (db.nextRow()) {
        int r = rowParner[db.getInt("f_partner")];
        ui->tbl->checkBox(r, 5)->setChecked(db.getInt("f_1") == 1);
        ui->tbl->checkBox(r, 6)->setChecked(db.getInt("f_2") == 1);
        ui->tbl->checkBox(r, 7)->setChecked(db.getInt("f_3") == 1);
        ui->tbl->checkBox(r, 8)->setChecked(db.getInt("f_4") == 1);
        ui->tbl->checkBox(r, 9)->setChecked(db.getInt("f_5") == 1);
        ui->tbl->checkBox(r, 10)->setChecked(db.getInt("f_6") == 1);
        ui->tbl->checkBox(r, 11)->setChecked(db.getInt("f_7") == 1);
    }
}

C5Route::~C5Route()
{
    delete ui;
}

QToolBar *C5Route::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5Route::on_leFilter_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool rowHidden = true;
        for (int c = 2; c < 5; c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                rowHidden = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, rowHidden);
    }
}

void C5Route::saveDataChanges()
{
    QString sql;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!sql.isEmpty()) {
            sql += ",";
        }
        sql += QString("(%1, %2, %3, %4, %5, %6, %7, %8)")
                .arg(ui->tbl->getString(i, 1),
                     ui->tbl->checkBox(i, 5)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 6)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 7)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 8)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 9)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 10)->isChecked() ? "1" : "0",
                     ui->tbl->checkBox(i, 11)->isChecked() ? "1" : "0");
    }
    sql = QString("insert into o_route (f_partner, f_1, f_2, f_3, f_4, f_5, f_6, f_7) values %1")
            .arg(sql);

    C5Database db(fDBParams);
    db.startTransaction();
    db.exec("delete from o_route");
    db.exec(sql);
    db.commit();
    C5Message::info(tr("Saved"));
}
