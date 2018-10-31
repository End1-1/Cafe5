#include "workspace.h"
#include "ui_workspace.h"
#include "c5connection.h"
#include "dlgpassword.h"

Workspace::Workspace(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::Workspace)
{
    ui->setupUi(this);
    fTypeFilter = 0;
}

Workspace::~Workspace()
{
    delete ui;
}

void Workspace::login()
{
    qDebug() << fDBParams;
    if (fDBParams.count() == 0) {
        go<C5Connection>(fDBParams);
        return;
    }
    if (!DlgPassword::getUserDB(tr("ENTER"), &fUser)) {
        accept();
        return;
    }
    ui->lbStaffName->setText(fUser.fFull);
    C5Database db(fDBParams);
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("select f_id, f_name \
            from d_part2 \
            where f_id in (select f_part from d_dish where f_id in (Select f_dish from d_menu where f_menu=:f_menu)) \
            order by f_queue");
    int row = 0, col = 0;
    while (db.nextRow()) {
        QTableWidgetItem *item = new QTableWidgetItem(db.getString(1));
        item->setData(Qt::UserRole, db.getInt(0));
        ui->tblPart2->setItem(row, col, item);
        col++;
        if (col == 6) {
            row++;
            col = 0;
        }
        if (row == 2) {
            break;
        }
    }
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("SELECT d.f_id, d.f_part, d.f_name,  m.f_print1, m.f_store, m.f_price, p2.f_adgcode \
            FROM d_menu m \
            left join d_dish d on d.f_id=m.f_dish \
            left join d_part2 p2 on p2.f_id=d.f_part \
            where m.f_menu=:f_menu \
            order by d.f_queue ");
    while (db.nextRow()) {
        Dish *d = new Dish();
        d->id = db.getInt(0);
        d->typeId = db.getInt(1);
        d->name = db.getString(2);
        d->printer = db.getString(3);
        d->price = db.getDouble(5);
        d->store = db.getInt(4);
        d->adgCode = db.getString(6);
        fDishes.append(d);
    }
    db[":f_id"] = C5Config::defaultTable();
    db.exec("select f_name from h_tables where f_id=:f_id");
    if (db.nextRow()) {
        ui->lbTable->setText(db.getString(0));
    }
    filter();
    stretchTableColumns(ui->tblDishes);
    stretchTableColumns(ui->tblOrder);
    stretchTableColumns(ui->tblPart2);
}

void Workspace::filter()
{
    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);
    int row = 0, col = 0;
    foreach (Dish *d, fDishes) {
        if (row > ui->tblDishes->rowCount() - 1) {
            ui->tblDishes->setRowCount(row + 1);
            for (int i = 0; i < ui->tblDishes->columnCount(); i++) {
                ui->tblDishes->setItem(row, i, new QTableWidgetItem());
            }
        }
        if (fTypeFilter > 0) {
            if (d->typeId != fTypeFilter) {
                continue;
            }
        }
        QTableWidgetItem *item = ui->tblDishes->item(row, col++);
        item->setText(d->name);
        item->setData(Qt::UserRole, qVariantFromValue(d));
        if (col > ui->tblDishes->columnCount() - 1) {
            col = 0;
            row++;
        }
    }

}

void Workspace::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        total += d.price * d.qty;
    }
    ui->leTotal->setDouble(total);
}

void Workspace::stretchTableColumns(QTableWidget *t)
{
    int freeSpace = t->width() - (t->columnCount() * t->horizontalHeader()->defaultSectionSize()) - 5;
    int delta = freeSpace / t->columnCount();
    t->horizontalHeader()->setDefaultSectionSize(delta + t->horizontalHeader()->defaultSectionSize());
}


void Workspace::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->data(Qt::UserRole).toInt() == 0) {
        return;
    }
    fTypeFilter = item->data(Qt::UserRole).toInt();
    filter();
}

void Workspace::on_tblDishes_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    Dish *d = item->data(Qt::UserRole).value<Dish*>();
    if (!d) {
        return;
    }
    Dish nd = *d;
    int row = ui->tblOrder->rowCount();
    ui->tblOrder->setRowCount(row + 1);
    ui->tblOrder->setItem(row, 0, new QTableWidgetItem(nd.name));
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, qVariantFromValue(nd));
    countTotal();
    ui->tblOrder->setCurrentCell(row, 0);
}

void Workspace::on_btnCheckout_clicked()
{
    C5Database db(fDBParams);
    db[":f_prefix"] = C5Config::orderPrefix();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = C5Config::defaultHall();
    db[":f_table"] = C5Config::defaultTable();
    db[":f_dateOpen"] = QDate::currentDate();
    db[":f_dateClose"] = QDate::currentDate();
    db[":f_timeOpen"] = QTime::currentTime();
    db[":f_timeClose"] = QTime::currentTime();
    db[":f_dateCash"] = QDate::currentDate();
    db[":f_staff"] = fUser.fId;
    db[":f_comment"] = "";
    db[":f_print"] = 1;
    db[":f_amounttotal"] = ui->leTotal->getDouble();
    db[":f_amountcash"] = ui->leTotal->getDouble();
    db[":f_amountcard"] = 0;
    db[":f_amountbank"] = 0;
    db[":f_amountother"] = 0;
    db[":f_amountservice"] = 0;
    db[":f_amountdiscount"] = 0;
    db[":f_servicefactor"] = 0;
    db[":f_discountfactor"] = 0;
    int id = db.insert("o_header");
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        db[":f_header"] = id;
        db[":f_state"] = DISH_STATE_OK;
        db[":f_dish"] = d.id;
        db[":f_qty1"] = d.qty;
        db[":f_qty2"] = d.qty;
        db[":f_price"] = d.price;
        db[":f_service"] = C5Config::serviceFactor().toDouble();
        db[":f_discount"] = 0;
        db[":f_total"] = d.qty * d.price;
        db[":f_store"] = d.store;
        db[":f_print1"] = d.printer;
        db[":f_print2"] = "";
        db[":f_comment"] = "";
        db[":f_remind"] = 0;
        db[":f_adgcode"] = d.adgCode;
        db.insert("o_body", false);
    }
    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(0);
    ui->leTotal->setDouble(0);
}

void Workspace::on_btnClearFilter_clicked()
{
    fTypeFilter = 0;
    filter();
}

void Workspace::on_btnVoid_clicked()
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    Dish d = ui->tblOrder->item(ml.at(0).row(), 0)->data(Qt::UserRole).value<Dish>();
    if (C5Message::question(tr("Confirm to remove") + "<br>" + d.name) != QDialog::Accepted) {
        return;
    }
    ui->tblOrder->removeRow(ml.at(0).row());
    countTotal();
}
