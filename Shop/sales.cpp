#include "sales.h"
#include "ui_sales.h"
#include "c5config.h"
#include "printreceipt.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5message.h"
#include "vieworder.h"

Sales::Sales(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Sales)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 120, 120, 100, 120, 120, 150, 300);
    refresh();
}

Sales::~Sales()
{
    delete ui;
}

void Sales::showSales(QWidget *parent)
{
    Sales *s = new Sales(parent);
    s->showMaximized();
    s->exec();
    delete s;
}

void Sales::on_btnDateLeft_clicked()
{
    changeDate(-1);
}

void Sales::changeDate(int d)
{
    ui->deStart->setDate(ui->deStart->date().addDays(d));
    ui->deEnd->setDate(ui->deEnd->date().addDays(d));
    refresh();
}

void Sales::refresh()
{
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select oh.f_id, oh.f_saletype, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, oh.f_amounttotal, concat(c.f_taxname, ' ', c.f_contact) as f_client "
            "from o_header oh "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=d.f_client "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "where oh.f_datecash between :f_start and :f_end and f_state=:f_state "
            "and oh.f_hall=:f_hall "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        row++;
    }
    int acol = 7;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
    double retail = 0, whosale = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->getInteger(i, 1) == SALE_RETAIL) {
            retail += ui->tbl->getDouble(i, acol);
        } else {
            whosale += ui->tbl->getDouble(i, acol);
        }
    }
    ui->leRetail->setDouble(retail);
    ui->leWhosale->setDouble(whosale);
}

void Sales::on_btnDateRight_clicked()
{
    changeDate(1);
}

void Sales::on_btnPrint_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    PrintReceipt pr(this);
    C5Database db(C5Config::dbParams());
    pr.print(ui->tbl->getString(ml.at(0).row(), 0), db);
}

void Sales::on_btnItemBack_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        C5Message::error(tr("No row selected"));
        return;
    }
    if (ui->tbl->getDouble(l.at(0).row(), 4) < 0) {
        C5Message::error(tr("Return inpossible to return, just view"));
    }
    ViewOrder *vo = new ViewOrder(ui->tbl->getString(l.at(0).row(), 0));
    vo->exec();
    vo->deleteLater();
}
