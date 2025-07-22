#include "sessionorders.h"
#include "ui_sessionorders.h"
#include "c5config.h"
#include "c5database.h"
#include "c5utils.h"

SessionOrders::SessionOrders() :
    C5Dialog(),
    ui(new Ui::SessionOrders)
{
    ui->setupUi(this);
    ui->tblOrders->setColumnWidth(0, 0);
    ui->tblOrders->setColumnWidth(1, 130);
    ui->tblOrders->setColumnWidth(2, 130);
    ui->tblOrders->setColumnWidth(3, 130);
    loadOrders();
}

SessionOrders::~SessionOrders()
{
    delete ui;
}

void SessionOrders::loadOrders()
{
    C5Database db;
    db[":f_session"] = __c5config.getRegValue("session").toInt();
    db.exec("SELECT o.f_id, CONCAT(f_prefix, f_hallid) as f_number, "
            "cast(CONCAT(o.f_dateclose, ' ', f_timeclose) as datetime) as f_date, o.f_amounttotal, "
            "ot.f_receiptnumber "
            "FROM o_header o "
            "LEFT JOIN a_header_cash c ON c.f_oheader=o.f_id "
            "left join o_tax ot on ot.f_id=o.f_id "
            "WHERE c.f_session=:f_session ");
    while (db.nextRow()) {
        int r = ui->tblOrders->rowCount();
        ui->tblOrders->setRowCount(r + 1);
        ui->tblOrders->setItem(r, 0, new QTableWidgetItem(db.getString("f_id")));
        ui->tblOrders->setItem(r, 1, new QTableWidgetItem(db.getString("f_number")));
        ui->tblOrders->setItem(r, 2, new QTableWidgetItem(db.getDateTime("f_date").toString(FORMAT_DATETIME_TO_STR)));
        ui->tblOrders->setItem(r, 3, new QTableWidgetItem(float_str(db.getDouble("f_amounttotal"), 2)));
        ui->tblOrders->setItem(r, 4, new QTableWidgetItem(db.getString("f_receiptnumber")));
    }
}

void SessionOrders::on_btnClose_clicked()
{
    accept();
}
