#include "workspace.h"
#include "ui_workspace.h"
#include "c5connection.h"
#include "dlgpassword.h"
#include "dishitemdelegate.h"
#include "c5waiterorderdoc.h"
#include "dishpartitemdelegate.h"
#include "dishtableitemdelegate.h"
#include "c5storedraftwriter.h"
#include "calendar.h"
#include "payment.h"
#include "c5printing.h"
#include <QScrollBar>
#include <QInputDialog>
#include <QScreen>
#include <QFile>


Workspace::Workspace(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::Workspace)
{
    ui->setupUi(this);
    fTypeFilter = 0;
    ui->tblPart2->setItemDelegate(new DishPartItemDelegate());
    ui->tblOrder->setItemDelegate(new DishItemDelegate());
    ui->tblDishes->setItemDelegate(new DishTableItemDelegate());
    QRect r = qApp->screens().at(0)->geometry();
    switch (r.width()) {
    case 1280:
        ui->tblDishes->setColumnCount(5);
        break;
    case 1024:
        ui->tblDishes->setColumnCount(4);
        break;
    }
}

Workspace::~Workspace()
{
    delete ui;
}

bool Workspace::login()
{
    if (fDBParams.count() == 0) {
        go<C5Connection>(fDBParams);
        return false;
    }
    if (!DlgPassword::getUserDB(tr("ENTER"), &fUser)) {
        accept();
        return false;
    }
    ui->lbStaffName->setText(fUser.fFull);
    C5Database db(fDBParams);
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("select f_id, f_name, f_color \
            from d_part2 \
            where f_id in (select f_part from d_dish where f_id in (Select f_dish from d_menu where f_menu=:f_menu and f_state=1)) \
            order by f_queue");
    int row = 0, col = 0;
    ui->tblPart2->setRowCount(1);
    QTableWidgetItem *itemAll = new QTableWidgetItem(tr("All"));
    itemAll->setData(Qt::UserRole, -1);
    itemAll->setData(Qt::BackgroundColorRole, -1);
    ui->tblPart2->setItem(row, col, itemAll);
    col++;
    if (col == ui->tblPart2->columnCount()) {
        row++;
        col = 0;
    }
    while (db.nextRow()) {
        if (row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem(db.getString(1));
        item->setData(Qt::UserRole, db.getInt(0));
        item->setData(Qt::BackgroundColorRole, db.getInt("f_color"));
        ui->tblPart2->setItem(row, col, item);
        col++;
        if (col == ui->tblPart2->columnCount()) {
            row++;
            col = 0;
        }        
    }
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("SELECT d.f_id, d.f_part, d.f_name,  m.f_print1, m.f_store, m.f_price, p2.f_adgcode, d.f_color, \
            d.f_netweight, d.f_cost \
            FROM d_menu m \
            left join d_dish d on d.f_id=m.f_dish \
            left join d_part2 p2 on p2.f_id=d.f_part \
            where m.f_menu=:f_menu and m.f_state=1 \
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
        d->netWeight = db.getDouble("f_netweight");
        d->cost = db.getDouble("f_cost");
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
    return true;
}

void Workspace::reject()
{
    if (C5Message::question(tr("Confirm to close application"))) {
        C5Dialog::reject();
    }
}

void Workspace::setQty()
{
    Dish d;
    if (!currentDish(d)) {
        return;
    }
    QPushButton *b = static_cast<QPushButton*>(sender());
    d.qty = b->text().toDouble();
    setCurrentDish(d);
}

void Workspace::changeQty()
{
    Dish d;
    if (!currentDish(d)) {
        return;
    }
    QPushButton *b = static_cast<QPushButton*>(sender());
    double q = b->text().toDouble();
    if (d.qty + q < 0.0001) {
        removeDish();
        return;
    }
    d.qty += q;
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

void Workspace::printReport(const QDate &d1, const QDate &d2)
{
    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("End of day"));
    p.br();
    p.ctext(d1.toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(d2.toString(FORMAT_DATE_TO_STR));
    p.br();
    double total = 0;

    C5Database dd(fDBParams);
    dd[":f_datecash1"] = d1;
    dd[":f_datecash2"] = d2;
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd[":f_stated"] = DISH_STATE_OK;
    dd.exec("select d.f_name, sum(b.f_qty1) as f_qty, b.f_price, sum(b.f_qty1*b.f_price) as f_total "
            "from o_body b "
            "inner join o_header h on h.f_id=b.f_header "
            "left join d_dish d on d.f_id=b.f_dish "
            "where h.f_state=:f_stateh and b.f_state=:f_stated and h.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1, 3 ");
    p.setFontBold(false);
    p.setFontSize(22);
    while (dd.nextRow()) {
        if (p.checkBr(p.fLineHeight + 2)) {
            p.br();
        }
        total += dd.getDouble("f_total");
        p.ltext(dd.getString("f_name"), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3").arg(float_str(dd.getDouble("f_qty"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_total"), 2)), 0);
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.setFontSize(28);
    p.ltext(tr("Total today"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}

void Workspace::stretchTableColumns(QTableWidget *t)
{
    int freeSpace = t->width() - (t->columnCount() * t->horizontalHeader()->defaultSectionSize()) - 5;
    int delta = freeSpace / t->columnCount();
    t->horizontalHeader()->setDefaultSectionSize(delta + t->horizontalHeader()->defaultSectionSize());
}

void Workspace::scrollTable(QTableWidget *t, int direction, int rows)
{
    t->verticalScrollBar()->setValue((t->verticalScrollBar()->value() + (t->verticalHeader()->defaultSectionSize() * rows)) * direction);
}


void Workspace::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
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
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish dd = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        if (dd.id == nd.id) {
            dd.qty += 1;
            ui->tblOrder->item(i, 0)->setData(Qt::UserRole, qVariantFromValue(dd));
            countTotal();
            return;
        }
    }
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
    QString prefix;
    QString hallid;
    db[":f_id"] = C5Config::defaultHall();
    db.exec("select f_counterhall from h_halls where f_id=:f_id");
    if (db.nextRow()) {
        int hid = db.getInt(0);
        db[":f_id"] = hid;
        db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
        if (db.nextRow()) {
            hallid = db.getString(0);
            prefix = db.getString(1);
            db[":f_counter"] = db.getInt(0);
            db.update("h_halls", where_id(hid));
        }
    } else {
        hallid ="[-]";
    }

    QDate dateCash = QDate::currentDate();
    QTime tc = QTime::fromString(__c5config.getValue(param_working_date_change_time), "00:00:00");
    if (tc.isValid()) {
        if (QTime::currentTime() < tc) {
            dateCash = dateCash.addDays(-1);
        }
    }
    db.startTransaction();
    C5StoreDraftWriter dw(db);
    QString id;
    if (!dw.writeOHeader(id, hallid.toInt(), prefix, ORDER_STATE_CLOSE, __c5config.defaultHall().toInt(),
                         __c5config.defaultTable(), QDate::currentDate(), QDate::currentDate(), dateCash,
                         QTime::currentTime(), QTime::currentTime(), fUser.fId, "", 1, ui->leTotal->getDouble(), ui->leTotal->getDouble(),
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0)) {
        C5Message::error(dw.fErrorMsg);
        db.rollback();
        return;
    }

    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        QString bid;
        if (!dw.writeOBody(bid, id, DISH_STATE_OK, d.id, d.qty, d.qty, d.price, d.qty*d.price, __c5config.serviceFactor().toDouble(), 0, d.store, d.printer, "", "", 0, d.adgCode, 0, 0, 0)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        if (!dw.writeOBodyToOGoods(bid, id)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
    }

    QString err;
    if (!dw.writeFromShopOutput(id, DOC_STATE_SAVED, err)) {
        C5Message::error(err);
        db.rollback();
        return;
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
    //p->exec();
    p->justPrint();
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

void Workspace::on_btnExit_clicked()
{
    if (C5Message::question(tr("Confirm to close application"))) {
        accept();
    }
}

void Workspace::on_btnPartUp_clicked()
{
    scrollTable(ui->tblPart2, -1, 2);
}

void Workspace::on_btnPartDown_clicked()
{
    scrollTable(ui->tblPart2, 1, 2);
}

void Workspace::on_btnDishUp_clicked()
{
    scrollTable(ui->tblDishes, -1, 5);
}

void Workspace::on_btnDishDown_clicked()
{
    scrollTable(ui->tblDishes, 1, 5);
}

void Workspace::on_btnAny_clicked()
{
    bool ok = false;
    double newQty = QInputDialog::getDouble(this, tr("Enter qty"), tr("Qty"), 0, 0.01, 99, 2, &ok);
    if (ok && newQty > 0.001) {
        Dish d;
        if (!currentDish(d)) {
            return;
        }
        d.qty = newQty;
        setCurrentDish(d);
    }
}

void Workspace::on_btnPrintReport_clicked()
{
    QDate date;
    if (!Calendar::getDate(date)) {
        return;
    }
    printReport(date, date);
}

void Workspace::on_btnPrintReport2_clicked()
{
    QDate date1, date2;
    if (!Calendar::getDate2(date1, date2)) {
        return;
    }
    printReport(date1, date2);
}
