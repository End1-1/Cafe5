#include "dlgmenuset.h"
#include "ui_dlgmenuset.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5database.h"
#include <QPushButton>

DlgMenuSet::DlgMenuSet(const QString &bodyId) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgMenuSet)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    ui->t1->configColumns({4, 0, 300, 100, 50});
    ui->t2->configColumns({5, 0, 0, 300, 100, 50});
    setProperty("bodyid", bodyId);
    C5Database db(fDBParams);
    db[":f_id"] = bodyId;
    db.exec("select ds.f_part, ds.f_qty, d.f_name, d2.f_setqty "
            "from d_dish_set ds "
            "left join d_dish d on d.f_id=ds.f_part "
            "left join d_dish d2 on d2.f_id=ds.f_dish "
            "where ds.f_dish=(select f_dish from o_body where f_id=:f_id)");
    while (db.nextRow()) {
        fMax = db.getDouble("f_setqty");
        int row = ui->t1->addEmptyRow();
        ui->t1->setInteger(row, 0, db.getInt("f_part"));
        ui->t1->setString(row, 1, db.getString("f_name"));
        ui->t1->setDouble(row, 2, db.getDouble("f_qty"));
        QPushButton *b = new QPushButton();
        ui->t1->setCellWidget(row, 3, b);
        b->setProperty("row", row);
        b->setIcon(QIcon(":/add.png"));
        connect(b, &QPushButton::clicked, this, &DlgMenuSet::addDish);
    }
    if (ui->t1->rowCount() > 0) {
        db[":f_id"] = bodyId;
        db[":f_set"] = bodyId;
        db.exec("update o_body set f_set=:f_set where f_id=:f_id");
    }
    db[":f_id"] = bodyId;
    db.exec("select f_header, f_store, f_print1, f_print2, f_qty1, f_row from o_body where f_id=:f_id");
    if (db.nextRow()) {
        setProperty("header", db.getValue("f_header"));
        setProperty("store", db.getValue("f_store"));
        setProperty("print1", db.getValue("f_print1"));
        setProperty("print2", db.getValue("f_print2"));
        setProperty("row", db.getValue("f_row"));
        fMax *= db.getDouble("f_qty1");
    }
    ui->lbMax->setText(QString("0/%1").arg(float_str(fMax, 1)));
    db[":f_set"] = bodyId;
    db[":f_state"] = DISH_STATE_SET;
    db.exec("select b.f_id, b.f_dish, d.f_name, b.f_qty1 "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_set=:f_set and b.f_set<>b.f_id and b.f_state=:f_state ");
    while (db.nextRow()) {
        int row = ui->t2->addEmptyRow();
        ui->lbMax->setText(QString("%1/%2").arg(ui->t2->rowCount()).arg(float_str(fMax, 1)));
        ui->t2->setString(row, 0, db.getString("f_id"));
        ui->t2->setInteger(row, 1, db.getInt("f_dish"));
        ui->t2->setString(row, 2, db.getString("f_name"));
        ui->t2->setDouble(row, 3, db.getDouble("f_qty1"));
    }
}

DlgMenuSet::~DlgMenuSet()
{
    delete ui;
}

void DlgMenuSet::on_toolButton_clicked()
{
    for (int i = 0; i < ui->t2->rowCount(); i++) {
        if (!ui->t2->getString(i, 0).isEmpty()) {
            continue;
        }
        QString id = C5Database::uuid();
        C5Database db(fDBParams);
        db[":f_id"] = id;
        db[":f_header"] = property("header");
        db[":f_state"] = DISH_STATE_SET;
        db[":f_dish"] = ui->t2->getInteger(i, 1);
        db[":f_qty1"] = 1;
        db[":f_qty2"] = 0;
        db[":f_price"] = 0;
        db[":f_service"] = 0;
        db[":f_discount"] = 0;
        db[":f_total"] = 0;
        db[":f_store"] = property("store");
        db[":f_print1"] = property("print1");
        db[":f_print2"] = property("print2");
        db[":f_comment"] = "";
        db[":f_comment2"] = "";
        db[":f_remind"] = 0;
        db[":f_adgcode"] = "";
        db[":f_removereason"] = "";
        db[":f_timeorder"] = 1;
        db[":f_package"] = 0;
        db[":f_candiscount"] = 0;
        db[":f_canservice"] = 0;
        db[":f_guest"] = 1;
        db[":f_set"] = property("bodyid");
        db[":f_row"] = property("row");
        db[":f_appendtime"] = QDateTime::currentDateTime();
        if (!db.insert("o_body", false)) {
            C5Message::error(db.fLastError);
        }
    }
    accept();
}

void DlgMenuSet::addDish()
{
    if (ui->t2->rowCount() >= fMax) {
        C5Message::error("Լիմիտը սպառվել է");
        return;
    }
    int rd = sender()->property("row").toInt();
    int row = ui->t2->addEmptyRow();
    ui->lbMax->setText(QString("%1/%2").arg(ui->t2->rowCount()).arg(float_str(fMax, 1)));
    ui->t2->setInteger(row, 1, ui->t1->getInteger(rd, 0));
    ui->t2->setString(row, 2, ui->t1->getString(rd, 1));
    ui->t2->setDouble(row, 3, 1);
    QPushButton *b = new QPushButton();
    ui->t2->setCellWidget(row, 4, b);
    b->setIcon(QIcon(":/minus.png"));
    connect(b, &QPushButton::clicked, this, &DlgMenuSet::removeDish);
}

void DlgMenuSet::removeDish()
{
    auto *b = static_cast<QPushButton *>(sender());
    int row = -1;
    for (int i = 0; i < ui->t2->rowCount(); i++) {
        if (b == ui->t2->cellWidget(i, 4)) {
            row = i;
            break;
        }
    }
    if (row == -1) {
        return;
    }
    if (ui->t2->getString(row, 0).isEmpty()) {
        ui->t2->removeRow(row);
    }
}
