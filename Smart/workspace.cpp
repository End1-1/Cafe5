#include "workspace.h"
#include "ui_workspace.h"
#include "c5connection.h"
#include "dlgpassword.h"
#include "dishitemdelegate.h"
#include "dishtableitemdelegate.h"
#include "payment.h"
#include "c5printing.h"

Workspace::Workspace(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::Workspace)
{
    ui->setupUi(this);
    fTypeFilter = 0;
    ui->tblOrder->setItemDelegate(new DishItemDelegate());
    ui->tblDishes->setItemDelegate(new DishTableItemDelegate());
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
    db.exec("select f_id, f_name, f_color \
            from d_part2 \
            where f_id in (select f_part from d_dish where f_id in (Select f_dish from d_menu where f_menu=:f_menu)) \
            order by f_queue");
    int row = 0, col = 0;
    while (db.nextRow()) {
        if (row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem(db.getString(1));
        item->setData(Qt::UserRole, db.getInt(0));
        item->setData(Qt::BackgroundRole, QColor::fromRgb(db.getInt("f_color")));
        ui->tblPart2->setItem(row, col, item);
        col++;
        if (col == 6) {
            row++;
            col = 0;
        }        
    }
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("SELECT d.f_id, d.f_part, d.f_name,  m.f_print1, m.f_store, m.f_price, p2.f_adgcode, d.f_color \
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
        d->color = db.getInt("f_color");
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

void Workspace::setQty()
{
    Dish d;
    if (!currentDish(d)) {
        return;
    }
    QPushButton *b = static_cast<QPushButton*>(sender());
    d.qty = b->text().toInt();
    setCurrentDish(d);
}

void Workspace::changeQty()
{
    Dish d;
    if (!currentDish(d)) {
        return;
    }
    QPushButton *b = static_cast<QPushButton*>(sender());
    d.qty += b->text().toDouble();
    setCurrentDish(d);
}

void Workspace::removeDish()
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblOrder->removeRow(row);
    row--;
    if (row > 0) {
        ui->tblOrder->setCurrentCell(row, 0);
    }
    countTotal();
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
    if (ui->tblOrder->rowCount() == 0) {
        return;
    }
    C5Database db(fDBParams);
    QString id = C5Database::uuid();
    QString prefix;
    QString hallid;
    db[":f_id"] = C5Config::defaultHall();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    if (db.nextRow()) {
        hallid = db.getString(0);
        prefix = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(C5Config::defaultHall()));
    }

    db.startTransaction();
    db[":f_id"] = id;
    db[":f_prefix"] = prefix;
    db[":f_hallid"] = hallid;
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
    if (!db.insert("o_header", false)) {
        C5Message::error(db.fLastError);
        db.rollback();
        return;
    }
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        db[":f_id"] = C5Database::uuid();
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
        if (!db.insert("o_body", false)) {
            C5Message::error(db.fLastError);
            db.rollback();
            return;
        }
    }
    db.commit();

    QSet<QString> prn;
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        if (d.printer.length() > 0) {
            prn.insert(d.printer);
        }
    }
    for (const QString &s: prn) {
        QFont font(qApp->font());
        font.setPointSize(22);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
        p.setFont(font);
        p.setFontBold(true);
        p.ctext(tr("Receipt #") + QString("%1%2").arg(prefix).arg(hallid));
        p.br();
        p.setFontBold(false);
        p.line(3);
        p.br(3);
        p.br();
        for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
            if (d.printer == s) {
                p.ltext(d.name, 0);
                p.rtext(float_str(d.qty, 2));
                p.br();
            }
        }
        p.br();
        p.line(3);
        p.br(3);
        p.ltext(QString("%1 %2").arg(tr("Printed:")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
        p.br();
        p.ltext("_", 0);
        p.br();
        p.print(s, QPrinter::Custom);
    }

    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(0);
    ui->leTotal->setDouble(0);
    payment *p = new payment(id, fDBParams);
    p->exec();
    delete p;
}

void Workspace::on_btnClearFilter_clicked()
{
    fTypeFilter = 0;
    filter();
}

bool Workspace::currentDish(Dish &d)
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return false;
    }
    d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();
    return true;
}

void Workspace::setCurrentDish(Dish &d)
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, qVariantFromValue<Dish>(d));
    ui->tblOrder->viewport()->update();
    countTotal();
}
