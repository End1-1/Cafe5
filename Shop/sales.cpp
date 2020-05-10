#include "sales.h"
#include "ui_sales.h"
#include "c5config.h"
#include "printreceipt.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5message.h"
#include "vieworder.h"

#define VM_TOTAL 0
#define VM_ITEMS 1

Sales::Sales(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Sales)
{
    ui->setupUi(this);
    ui->lbRetail->setVisible(!__c5config.shopDenyF1());
    ui->leRetail->setVisible(!__c5config.shopDenyF1());
    ui->lbWhosale->setVisible(!__c5config.shopDenyF2());
    ui->leWhosale->setVisible(!__c5config.shopDenyF2());
    fViewMode = VM_TOTAL;
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
    switch (fViewMode) {
    case VM_TOTAL:
        refreshTotal();
        break;
    case VM_ITEMS:
        refreshItems();
        break;
    }
}

void Sales::refreshTotal()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Amount"));
    h.append(tr("Customer"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 120, 120, 100, 120, 120, 150, 300);
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

void Sales::refreshItems()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Price"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 120, 120, 100, 120, 120, 150, 300, 80, 80, 80);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select oh.f_id, oh.f_saletype, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, g.f_name as f_goodsname, og.f_qty, og.f_price, og.f_total "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
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
    int acol = 10;
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

void Sales::on_btnRefresh_clicked()
{
    refresh();
}

void Sales::on_btnModeTotal_clicked()
{
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(true);
    fViewMode = VM_TOTAL;
    refresh();
}

void Sales::on_btnModeItems_clicked()
{
    ui->btnModeItems->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    fViewMode = VM_ITEMS;
    refresh();
}
