#include "sales.h"
#include "ui_sales.h"
#include "c5config.h"
#include "printreceiptgroup.h"
#include "c5database.h"
#include "selectprinters.h"
#include "c5replication.h"
#include "c5utils.h"
#include "c5message.h"
#include "printtaxn.h"
#include "vieworder.h"
#include "cashcollection.h"

#define VM_TOTAL 0
#define VM_ITEMS 1
#define VM_TOTAL_ITEMS 2

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
    C5Database db(__c5config.dbParams());
    if (__c5config.rdbReplica()) {
        db.exec("select * from o_header");
        ui->btnRetryUpload->setVisible(db.nextRow());
    } else {
        ui->btnRetryUpload->setVisible(false);
    }
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
    case VM_TOTAL_ITEMS:
        refreshTotalItems();
        break;
    }
}

void Sales::refreshTotal()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Amount"));
    h.append(tr("Customer"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 80, 120, 120, 100, 120, 120, 150, 300);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select oh.f_id, oh.f_saletype, u.f_login, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, oh.f_amounttotal, concat(c.f_taxname, ' ', c.f_contact) as f_client "
            "from o_header oh "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=d.f_client "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
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
    int acol = 8;
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
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Price"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 80, 120, 120, 100, 120, 100, 150, 250, 80, 80, 80);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select oh.f_id, oh.f_saletype, u.f_login, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, g.f_scancode, g.f_name as f_goodsname, og.f_qty, og.f_price, og.f_total "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=d.f_client "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
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
    int acol = 11;
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

void Sales::refreshTotalItems()
{
    QStringList h;
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 150, 250, 80, 80);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select g.f_scancode, g.f_name as f_goodsname, sum(og.f_qty), sum(og.f_total) "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall "
            "group by 1, 2 "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        row++;
    }
    int acol = 3;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
    ui->leRetail->setDouble(0);
    ui->leWhosale->setDouble(0);
}

QString Sales::userCond() const
{
    if (!ui->leLogin->isEmpty()) {
        return " and u.f_login='" + ui->leLogin->text() + "' ";
    }
    return "";
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
    bool p1, p2;
    if (SelectPrinters::selectPrinters(p1, p2)) {
        PrintReceiptGroup p;
        C5Database db(C5Config::dbParams());
        if (p1) {
            p.print(ui->tbl->getString(ml.at(0).row(), 0), db, 1);
        }
        if (p2) {
            p.print(ui->tbl->getString(ml.at(0).row(), 0), db, 2);
        }
        //p.print2(oheaderid, db);
    }
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
    ui->btnTotalByItems->setChecked(false);
    ui->btnPrint->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->btnPrintTax->setEnabled(true);
    fViewMode = VM_TOTAL;
    refresh();
}

void Sales::on_btnModeItems_clicked()
{
    ui->btnModeItems->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(false);
    ui->btnPrint->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->btnPrintTax->setEnabled(false);
    fViewMode = VM_ITEMS;
    refresh();
}

void Sales::on_btnTotalByItems_clicked()
{
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(true);
    ui->btnPrint->setEnabled(false);
    ui->btnItemBack->setEnabled(false);
    ui->btnPrintTax->setEnabled(false);
    fViewMode = VM_TOTAL_ITEMS;
    refresh();
}

void Sales::on_btnPrintTax_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    if (ui->tbl->getInteger(ml.at(0).row(), 5) > 0) {
        C5Message::error(tr("Cannot print tax twice"));
        return;
    }
    if (C5Message::question(tr("Print tax") + "<br>" + float_str(ui->tbl->getDouble(ml.at(0).row(), 8), 2)) != QDialog::Accepted) {
        return;
    }
    QString id = ui->tbl->getString(ml.at(0).row(), 0);
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    double disc = db.getDouble("f_discountfactor");
    double card = db.getDouble("f_amountcard");
    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
    while (db.nextRow()) {
        pt.addGoods(db.getString("f_taxdept"), //dep
                    db.getString("f_adgcode"), //adg
                    db.getString("f_goods"), //goods id
                    db.getString("f_name"), //name
                    db.getDouble("f_price"), //price
                    db.getDouble("f_qty"), //qty
                    disc); //discount
    }
    QString jsonIn, jsonOut, err;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    int result = 0;
    result = pt.makeJsonAndPrint(card, 0, jsonIn, jsonOut, err);

    db[":f_id"] = db.uuid();
    db[":f_order"] = id;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    QSqlQuery *q = new QSqlQuery(db.fDb);
    if (result == pt_err_ok) {
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = id;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = id;
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax", false);
        pt.saveTimeResult(id, *q);
        db[":f_tax"] = rseq.toInt();
        db.update("o_goods", "f_header", id);
        delete q;
        ui->btnPrintTax->setVisible(false);
        ui->tbl->setString(ml.at(0).row(), 5, rseq);
        C5Message::info(tr("Printed"));
    } else {
        pt.saveTimeResult("Not saved - " + id, *q);
        delete q;
        C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
    }
}

void Sales::on_btnRetryUpload_clicked()
{
    C5Replication r;
    if (r.uploadToServer()) {
        ui->btnRetryUpload->setVisible(false);
    } else {
        C5Message::error(tr("Cannot upload data"));
    }
}

void Sales::on_btnCashColletion_clicked()
{
    auto *cc = new CashCollection();
    cc->exec();
    delete cc;
}

void Sales::on_leFilter_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool h = true;
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, h);
    }
}
