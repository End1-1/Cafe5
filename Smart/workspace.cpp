#include "workspace.h"
#include "ui_workspace.h"
#include "c5connection.h"
#include "dlgpassword.h"
#include "c5waiterorderdoc.h"
#include "menudish.h"
#include "orderdish.h"
#include "c5storedraftwriter.h"
#include "calendar.h"
#include "c5user.h"
#include "supplier.h"
#include "dishpackage.h"
#include "c5printing.h"
#include "touchdlgphonenumber.h"
#include "touchentertaxreceiptnumber.h"
#include "touchselecttaxreport.h"
#include "printtaxn.h"
#include "dlgqty.h"
#include "idram.h"
#include "change.h"
#include "menudialog.h"
#include "QRCodeGenerator.h"
#include "dlglistofdishcomments.h"
#include "customerinfo.h"
#include "thread.h"
#include <QScrollBar>
#include <QInputDialog>
#include <QScreen>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QColor>
#include <QTimer>
#include <QFile>

QHash<QString, QString> fPrinterAliases;


Workspace::Workspace(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::Workspace)
{
    ui->setupUi(this);
    fTypeFilter = 0;
    QRect r = qApp->screens().at(0)->geometry();
    switch (r.width()) {
    case 1280:
        ui->tblDishes->setColumnCount(3);
        break;
    case 1024:
        ui->tblDishes->setColumnCount(2);
        break;
    default:
        ui->tblDishes->setColumnCount(r.width() / 300);
        break;
    }
    if (__c5config.getValue(param_tax_print_always_offer).toInt() != 0) {
        ui->btnFiscal->setEnabled(false);
        ui->btnFiscal->setChecked(true);
    }
    resetOrder();
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=]() {
       ui->leReadCode->setFocus();
    });
    t->start(1000);
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
    if (!DlgPassword::getUser(tr("ENTER"), fUser)) {
        accept();
        return false;
    }
    ui->lbStaffName->setText(fUser->fullName());
    C5Database db(fDBParams);
    db.exec("select * from d_print_aliases");
    while (db.nextRow()) {
        fPrinterAliases[db.getString("f_alias")] = db.getString("f_printer");
    }
    db[":f_menu"] = C5Config::defaultMenu();
    db.exec("select f_id, f_name, f_color \
            from d_part2 \
            where f_id in (select f_part from d_dish where f_id in (Select f_dish from d_menu where f_menu=:f_menu and f_state=1)) \
            order by f_queue");
    int row = 0, col = 0;
    ui->tblPart2->setRowCount(1);
    QTableWidgetItem *itemAll = new QTableWidgetItem(tr("POPULAR"));
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
            d.f_netweight, d.f_cost, m.f_recent, d.f_barcode \
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
        d->quick = db.getInt("f_recent");
        d->barcode = db.getString("f_barcode");
        fDishes.append(d);
        if (d->barcode.isEmpty() == false) {
            fDishesBarcode[d->barcode] = d;
        }
    }
    db[":f_id"] = C5Config::defaultTable();
    db.exec("select f_name from h_tables where f_id=:f_id");
    if (db.nextRow()) {
        ui->lbTable->setText(db.getString(0));
    }
    db[":f_menu"] = __c5config.defaultMenu();
    db.exec("select p.f_package, p.f_dish, d.f_name, p.f_price, dg.f_adgcode, "
            "p.f_store, p.f_printer, p.f_qty "
            "from d_package_list p "
            "inner join d_dish d on d.f_id=p.f_dish "
            "inner join d_package dp on dp.f_id=p.f_package "
            "inner join d_part2 dg on dg.f_id=d.f_part "
            "where dp.f_menu=:f_menu and dp.f_enabled=1 ");
    while (db.nextRow()) {
        DishPackageDriver::fPackageDriver.addMember(db.getInt(0), db.getInt(1), db.getString(2),
                                                    db.getDouble(3), db.getDouble(7),
                                                    db.getString(4), db.getInt(5), db.getString(6));
    }
    db[":f_menu"] = __c5config.defaultMenu();
    db.exec("select f_id, f_name, f_price from d_package where f_menu=:f_menu and f_enabled=1");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstCombo);
        item->setText(db.getString(1));
        item->setData(Qt::UserRole, db.getInt(0));
        item->setData(Qt::UserRole + 1, db.getDouble(2));
        item->setSizeHint(QSize(ui->lstCombo->width() - 5, DishPackageDriver::fPackageDriver.itemHeight(db.getInt(0), ui->lstCombo->width(), item->text())));
        ui->lstCombo->addItem(item);
        ui->lstCombo->setItemDelegate(new DishMemberDelegate());
    }
    ui->btnShowPackages->setVisible(ui->lstCombo->count() > 0);
    filter();
    stretchTableColumns(ui->tblDishes);
    stretchTableColumns(ui->tblOrder);
    stretchTableColumns(ui->tblPart2);
    on_btnShowDishes_clicked();
    return true;
}

void Workspace::reject()
{
    if (C5Message::question(tr("Confirm to close application"))) {
        C5Dialog::reject();
    }
}

void Workspace::setQty(double qty, int mode)
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return;
    }
    OrderDish *od = static_cast<OrderDish*>(ui->tblOrder->cellWidget(row, 0));
    Dish &d = od->fDish;
    switch (mode) {
    case 1:
        d.qty = qty;
        break;
    case 2:
        d.qty += qty;
        break;
    case 3:
        if (d.qty - qty > 0.1) {
            d.qty -= qty;
        } else {
            on_btnVoid_clicked();
        }
        break;
    }
    od->updateInfo();
    countTotal();
}

void Workspace::removeDish()
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblOrder->removeRow(row);
    if (row < ui->tblOrder->rowCount()) {
        on_tblOrder_cellClicked(row, 0);
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
        } else {
            if (d->quick == 0) {
                continue;
            }
        }
        MenuDish *m = new MenuDish(d);
        if (d->color != -1) {
            m->setStyleSheet("background-color:" + QColor::fromRgb(d->color).name());
        }
        ui->tblDishes->setCellWidget(row, col, m);
        col++;
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
        Dish &d = static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, 0))->fDish;
        total += d.price * d.qty;
    }
    if (fDiscountMode > 0) {
        ui->wDiscount->setVisible(true);
        switch (fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            fDiscountAmount = total * fDiscountValue;
            ui->leDiscount->setText(QString("-%1% (%2)").arg(fDiscountValue * 100).arg(fDiscountAmount));
            total -= fDiscountAmount;
            break;
        }
    }
    ui->leTotal->setDouble(total);
    if (ui->leReceived->getDouble() > 0.01) {
        ui->leChange->setDouble(ui->leReceived->getDouble() - ui->leTotal->getDouble());
    }
}

void Workspace::resetOrder()
{
    ui->tblDishes->setEnabled(true);
    ui->wQty->setEnabled(true);
    fOrderUuid.clear();
    ui->leInfo->setVisible(false);
    ui->wDiscount->setVisible(false);
    fSupplierId = __c5config.defaultTable();
    fSupplierName = "";
    fCustomer = 0;
    fDiscountAmount = 0;
    fDiscountMode = 0;
    fDiscountValue = 0;
    fDiscountId = 0;
    fSupplierId = __c5config.defaultTable();
    fSupplierName = "";
    fPhone = "";
    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(0);
    ui->leTotal->setDouble(0);
    ui->leReceived->setDouble(0);
    ui->leChange->setDouble(0);
    ui->leInfo->setVisible(false);
    fDiscountMode = 0;
    fDiscountValue = 0;
    fDiscountAmount = 0;
    ui->btnSetCash->click();
    setCustomerPhoneNumber("");
}

void Workspace::stretchTableColumns(QTableWidget *t)
{
    int hscrollwidth = 0;
    if(t->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) {
        hscrollwidth = 20;
    }
    int freeSpace = t->width() - (t->columnCount() * t->horizontalHeader()->defaultSectionSize()) - 6 - hscrollwidth;
    int delta = freeSpace / t->columnCount();
    t->horizontalHeader()->setDefaultSectionSize(delta + t->horizontalHeader()->defaultSectionSize());
    t->horizontalHeader()->setMinimumSectionSize(t->horizontalHeader()->defaultSectionSize());
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

void Workspace::on_btnCheckout_clicked()
{
    if (ui->tblOrder->rowCount() == 0) {
        return;
    }
    C5Database db(fDBParams);
    if (fOrderUuid.isEmpty()) {
        saveOrder();
    }

    if (ui->btnSetIdram->isChecked()) {
        double amount;
        QByteArray out;
        if (!Idram::check(__c5config.getValue(param_idram_server),
                          __c5config.getValue(param_idram_session_id),
                          fOrderUuid, amount, out)) {
            if (amount < 0) {
                C5Message::error(tr("Idram payment was not received"));
            } else {
                C5Message::error(out);
            }
            return;
        }
    }

    double cashAmount = 0, cardAmount = 0, idramAmount = 0, otherAmount = 0;
    if (ui->btnSetCash->isChecked()) {
        cashAmount = ui->leTotal->getDouble();
    } else if (ui->btnSetCard->isChecked()) {
        cardAmount = ui->leTotal->getDouble();
    } else if (ui->btnSetIdram->isChecked()) {
        idramAmount = ui->leTotal->getDouble();
    } else {
        otherAmount = ui->leTotal->getDouble();
    }
    if (ui->btnFiscal->isChecked() && !ui->btnSetOther->isChecked()) {
        while (!printTax(cardAmount, idramAmount)) {
            continue;
        }
    }

    db[":f_id"] = fOrderUuid;
    db.exec("select * from o_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(db.fLastError);
        return;
    }
    QString headerNum = db.getString("f_prefix") + db.getString("f_hallid");
    int cashid = ui->btnSetCash->isChecked() ? __c5config.cashId() : __c5config.nocashId();
    QString cashprefix = ui->btnSetCash->isChecked() ? __c5config.cashPrefix() : __c5config.nocashPrefix();

    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    if (counter == 0) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashdoc;
    if (!dw.writeAHeader(cashdoc, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH,
                         fUser->id(), QDate::currentDate(), QDate::currentDate(), QTime::currentTime(),
                         0, ui->leTotal->getDouble(), cashprefix + " " + headerNum)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    if (!dw.writeAHeaderCash(cashdoc, cashid, 0, 1, "", fOrderUuid)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdoc, cashid, 1, cashprefix + " " + headerNum, ui->leTotal->getDouble(), cashUUID, 1)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    fPreviouseUuid = fOrderUuid;
    if (__c5config.getValue(param_smart_dont_print_receipt).toInt() == 0) {
        if (printReceipt(fOrderUuid, true)) {

        }
    }

    if (__c5config.getValue(param_waiter_automatially_storeout).toInt() == 1) {
        auto *t = new Thread("C5WaiterOrderDoc");
        C5WaiterOrderDoc *d = new C5WaiterOrderDoc(fOrderUuid, db, true);
        connect(t, &QThread::started, d, &C5WaiterOrderDoc::run);
        connect(d, &C5WaiterOrderDoc::finished, t, &QThread::quit);
        connect(t, &QThread::finished, t, &QThread::deleteLater);
        d->moveToThread(t);
        t->start();
    }

    resetOrder();
}

void Workspace::on_btnClearFilter_clicked()
{
    fTypeFilter = 0;
    filter();
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
    double newQty = 0;
    if (DlgQty::getQty(newQty, "")) {
        setQty(newQty, 1);
    }
}

void Workspace::on_btnShowPackages_clicked()
{
    int h = ui->wDishPart->height() - ui->btnShowPackages->height() - ui->btnPartUp->height() - 6;
    QPropertyAnimation *p1 = new QPropertyAnimation(ui->lstCombo, "minimumHeight", this);
    QPropertyAnimation *p2 = new QPropertyAnimation(ui->lstCombo, "maximumHeight", this);
    QPropertyAnimation *p3 = new QPropertyAnimation(ui->tblPart2, "minimumHeight", this);
    QPropertyAnimation *p4 = new QPropertyAnimation(ui->tblPart2, "maximumHeight", this);

    p1->setStartValue(0);
    p1->setEndValue(h - 100);
    p2->setStartValue(0);
    p2->setEndValue(h);
    p3->setStartValue(h - 100);
    p3->setEndValue(0);
    p4->setStartValue(h);
    p4->setEndValue(0);

    p1->setDuration(100);
    p2->setDuration(100);
    p3->setDuration(100);
    p4->setDuration(100);
    QParallelAnimationGroup *ag = new QParallelAnimationGroup(this);
    ag->addAnimation(p1);
    ag->addAnimation(p2);
    ag->addAnimation(p3);
    ag->addAnimation(p4);
    ag->start();

    ui->btnShowPackages->setVisible(false);
    ui->btnShowDishes->setVisible(true);
}

void Workspace::on_btnShowDishes_clicked()
{
    int h = ui->wDishPart->height() - ui->btnShowDishes->height() - ui->btnPartUp->height() - 6;
    QPropertyAnimation *p1 = new QPropertyAnimation(ui->tblPart2, "minimumHeight", this);
    QPropertyAnimation *p2 = new QPropertyAnimation(ui->tblPart2, "maximumHeight", this);
    QPropertyAnimation *p3 = new QPropertyAnimation(ui->lstCombo, "minimumHeight", this);
    QPropertyAnimation *p4 = new QPropertyAnimation(ui->lstCombo, "maximumHeight", this);

    p1->setStartValue(0);
    p1->setEndValue(h - 100);
    p2->setStartValue(0);
    p2->setEndValue(h);
    p3->setStartValue(h - 100);
    p3->setEndValue(0);
    p4->setStartValue(h);
    p4->setEndValue(0);

    p1->setDuration(100);
    p2->setDuration(100);
    p3->setDuration(100);
    p4->setDuration(100);
    QParallelAnimationGroup *ag = new QParallelAnimationGroup(this);
    ag->addAnimation(p1);
    ag->addAnimation(p2);
    ag->addAnimation(p3);
    ag->addAnimation(p4);
    ag->start();

    ui->btnShowPackages->setVisible(true);
    ui->btnShowDishes->setVisible(false);
}

void Workspace::on_lstCombo_itemClicked(QListWidgetItem *item)
{
    const QList<DishPackageMember> &p = DishPackageDriver::fPackageDriver.fPackage[item->data(Qt::UserRole).toInt()];
//    int row = ui->tblOrder->rowCount();
//    ui->tblOrder->setRowCount(row + 1);
//    ui->tblOrder->setSpan(row, 0, 1, 2);
//    ui->tblOrder->setRowHeight(row, 30);
//    auto *packageItem = new QTableWidgetItem(item->text());
//    packageItem->setData(Qt::UserRole + 99, item->data(Qt::UserRole));
//    packageItem->setData(Qt::UserRole + 100, -1);
//    packageItem->setData(Qt::UserRole + 101, 1);
//    packageItem->setData(Qt::UserRole + 102, item->data(Qt::UserRole + 1));
//    ui->tblOrder->setItem(row, 0, packageItem);
    for (const DishPackageMember &dm: p) {        

        Dish nd;
        nd.id = dm.fDish;
        nd.adgCode = dm.fAdgCode;
        nd.name = dm.fName;
        nd.price = dm.fPrice;
        nd.store = dm.fStore;
        nd.printer = dm.fPrinter;
        nd.package = dm.fPackage;
        nd.qty = dm.fQty;

        int row = ui->tblOrder->rowCount();
        ui->tblOrder->setRowCount(row + 1);
        OrderDish *od = new OrderDish(nd);
        ui->tblOrder->setCellWidget(row, 0, od);

//        nd.packageName = item->data(Qt::DisplayRole).toString();
//        row = ui->tblOrder->rowCount();
//        ui->tblOrder->setRowCount(row + 1);
//        ui->tblOrder->setItem(row, 0, new QTableWidgetItem(nd.name));
//        ui->tblOrder->item(row, 0)->setData(Qt::UserRole, qVariantFromValue(nd));
//        ui->tblOrder->item(row, 0)->setData(Qt::UserRole + 98, -1);
    }
//    row = ui->tblOrder->rowCount();
//    ui->tblOrder->setRowCount(row + 1);
//    ui->tblOrder->setSpan(row, 0, 1, 2);
//    ui->tblOrder->setRowHeight(row, 10);
//    packageItem = new QTableWidgetItem(item->text());
//    packageItem->setData(Qt::UserRole + 100, -2);
//    ui->tblOrder->setItem(row, 0, packageItem);
    countTotal();
}

void Workspace::on_leReadCode_returnPressed()
{
    QString hya("էթփձջւևրչճԷԹՓՁՋՒևՐՉՃ");
    QString num("12345678901234567890");
    QString oldcode = ui->leReadCode->text();
    ui->leReadCode->clear();
    QString newcode;

    for (int i = 0; i < oldcode.length(); i++) {
        if (hya.contains(oldcode.at(i))) {
            newcode += num.at(hya.indexOf(oldcode.at(i)));
        } else {
            newcode += oldcode.at(i);
        }
    }

    QString code = newcode.replace("?", "").replace(";", "");
    if (fDishesBarcode.contains(code)) {
        addDishToOrder(fDishesBarcode[code]);
        return;
    }

    C5Database db(fDBParams);
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (db.nextRow()) {
        fDiscountMode = db.getInt("f_mode");
        fDiscountValue = db.getDouble("f_value");
        switch (fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            fDiscountValue /= 100;
            break;
        }

        countTotal();
    }
}

void Workspace::on_btnCostumer_clicked()
{
//    QString phone;
//    if (TouchDlgPhoneNumber::getPhoneNumber(phone)) {
//        setCustomerPhoneNumber(phone);
//    }
    if (ui->lbCostumerPhone->isVisible()) {
        if (C5Message::question(tr("Confirm to remove customer informaion")) == QDialog::Accepted) {
            fCustomer = 0;
            ui->lbCostumerPhone->setVisible(false);
            return;
        }
    }
    QString phone, address, name;
    int id;
    if (CustomerInfo::getCustomer(id, name, phone, address)) {
        fCustomer = id;
        ui->lbCostumerPhone->setText(QString("%1\r\n%2\r\n%3").arg(phone, name, address));
        ui->lbCostumerPhone->setVisible(true);
    }
}

void Workspace::setCustomerPhoneNumber(const QString &number)
{
    ui->lbCostumerPhone->setText(number);
    ui->lbCostumerPhone->setVisible(number.isEmpty() == false);
}

void Workspace::on_tblDishes_cellClicked(int row, int column)
{
    MenuDish *md = static_cast<MenuDish*>(ui->tblDishes->cellWidget(row, column));
    if (md == nullptr) {
        return;
    }
    Dish *d = md->fDish;
    if (!d) {
        return;
    }
    addDishToOrder(d);
}

void Workspace::on_btnP1_clicked()
{
    setQty(1, 2);
}

void Workspace::on_btnM1_clicked()
{
    setQty(1, 3);
}

void Workspace::on_btnP05_clicked()
{
    setQty(0.5, 1);
}

void Workspace::on_tblOrder_cellClicked(int row, int column)
{
    ui->tblOrder->setCurrentCell(row, 0);
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, column))->setSelected(row == i);
    }
}

void Workspace::on_btnVoid_clicked()
{
    removeDish();
}

void Workspace::on_btnSetCash_clicked()
{
    ui->btnSetCash->setChecked(true);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(false);
}

void Workspace::on_btnSetCard_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(true);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(false);
}

void Workspace::on_btnSetIdram_clicked()
{
    if (ui->tblOrder->rowCount() == 0) {
        on_btnSetCash_clicked();
        C5Message::info(tr("Empty order"));
        return;
    }
    if (fOrderUuid.isEmpty() && C5Message::question(tr("Order will saved, continue?")) != QDialog::Accepted) {
        on_btnSetCash_clicked();
        return;
    }
    if (fOrderUuid.isEmpty()) {
        saveOrder();
    }
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(true);
    ui->btnSetOther->setChecked(false);

    if (__c5config.getValue(param_idram_id).length() > 0 && !__c5config.localReceiptPrinter().isEmpty()) {
        QFont font(qApp->font());
        font.setPointSize(24);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
        p.setFont(font);
        p.setFontBold(true);
        p.ctext(fOrderUuid);
        p.br();
        p.setFontBold(false);
        p.line(3);
        p.br(3);
        p.br();
        p.br();
        p.ctext(tr("Amount to paid"));
        p.br();
        p.ctext(ui->leTotal->text());
        p.br();
        p.br();
        p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
        p.br();

        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
                .arg(__c5config.getValue(param_idram_name))
                .arg(__c5config.getValue(param_idram_id)) //IDram ID
                .arg(str_float(ui->leTotal->text()))
                .arg(fOrderUuid)
                .arg(__c5config.getValue(param_idram_phone))
                .arg(__c5config.getValue(param_idram_tips).toInt() == 1 ? "1" : "0")
                .arg(__c5config.getValue(param_idram_tips).toInt() == 1 ? __c5config.getValue(param_idram_tips_wallet) : "");

        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
        if (!successfulEncoding) {
//            fLog.append("Cannot encode qr image");
        }
        int qrImageSize = qrEncode.m_nSymbleSize;
        int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
        encodeImage.fill(1);

        for ( int i = 0; i < qrImageSize; i++ ) {
            for ( int j = 0; j < qrImageSize; j++ ) {
                if ( qrEncode.m_byModuleData[i][j] ) {
                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(encodeImage);
        pix = pix.scaled(300, 300);
        p.image(pix, Qt::AlignHCenter);
        p.br();
        /* End QRCode */
        p.ltext(QString("%1 %2").arg(tr("Printed:")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
        p.br();
        p.ltext("_", 0);
        p.br();
        p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    }
}

void Workspace::on_btnSetOther_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(true);
}

void Workspace::on_btnReceived_clicked()
{
    double v = ui->leTotal->getDouble();
    if (Change::getReceived(v)) {
        ui->leReceived->setDouble(v);
    }
}

void Workspace::on_leReceived_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(str_float(arg1) - ui->leTotal->getDouble());
}

void Workspace::saveOrder()
{
    QString prefix;
    QString hallid;
    C5Database db(fDBParams);
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
        hallid = "[-]";
    }

    QDate dateCash = QDate::currentDate();
    QTime tc = QTime::fromString(__c5config.getValue(param_working_date_change_time), "00:00:00");
    if (tc.isValid()) {
        if (QTime::currentTime() < tc) {
            dateCash = dateCash.addDays(-1);
        }
    }

    double cashAmount = 0, cardAmount = 0, idramAmount = 0, otherAmount = 0;
    if (ui->btnSetCash->isChecked()) {
        cashAmount = ui->leTotal->getDouble();
    } else if (ui->btnSetCard->isChecked()) {
        cardAmount = ui->leTotal->getDouble();
    } else if (ui->btnSetIdram->isChecked()) {
        idramAmount = ui->leTotal->getDouble();
    } else {
        otherAmount = ui->leTotal->getDouble();
    }

    db.startTransaction();
    C5StoreDraftWriter dw(db);
    if (!dw.writeOHeader(fOrderUuid,
                         hallid.toInt(), prefix, ORDER_STATE_CLOSE,
                         __c5config.defaultHall(), fSupplierId,
                         QDate::currentDate(), QDate::currentDate(), dateCash,
                         QTime::currentTime(), QTime::currentTime(),
                         fUser->id(), fPhone, 0,
                         ui->leTotal->getDouble(), cashAmount, cardAmount, 0, 0, otherAmount, idramAmount,
                         0, fDiscountAmount, 0, fDiscountValue,
                         0, 0, 1, 1, 1, fCustomer)) {
        C5Message::error(dw.fErrorMsg);
        db.rollback();
        return;
    }

    if (fCustomer > 0) {
        dw.writeOHeaderFlags(fOrderUuid, 1, 0, 0, 0, 0);
    }

    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish &d = static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, 0))->fDish;
            int pid = 0;
            QString bid;
            double disc = 0;
            switch (fDiscountMode) {
            case CARD_TYPE_DISCOUNT:
                disc = fDiscountValue;
                break;
            default:
                break;
            }
            if (!dw.writeOBody(bid, fOrderUuid, DISH_STATE_OK, d.id, d.qty, d.qty, d.price, (d.qty*d.price) - (d.qty*d.price*disc), __c5config.serviceFactor().toDouble(), fDiscountValue, d.store, d.printer, "", "", 0, d.adgCode, 0, 0, pid)) {
                C5Message::error(dw.fErrorMsg);
                db.rollback();
                return;
            }
//            if (!dw.writeOBodyToOGoods(bid, id)) {
//                C5Message::error(dw.fErrorMsg);
//                db.rollback();
//                return;
//            }

    }


//    QString err;
//    if (!dw.writeFromShopOutput(id, DOC_STATE_SAVED, err)) {
//        C5Message::error(err);
//        db.rollback();
//        return;
//    }
    db.commit();

    QSet<QString> prn;
    for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish &d = static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, 0))->fDish;
        if (d.printer.length() > 0) {
            prn.insert(d.printer);
        }
    }

    for (const QString &s: prn) {
        QFont font(qApp->font());
        font.setFamily(__c5config.getValue(param_service_print_font_family));
        int basesize = __c5config.getValue(param_service_print_font_size).toInt();
        font.setPointSize(basesize);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
        p.setFont(font);
        p.setFontBold(true);
        p.ctext(tr("Receipt #") + QString("%1%2").arg(prefix, hallid));
        p.br();
        if (fCustomer > 0) {
            p.ctext(QString("*%1*").arg(tr("Delivery")));
            p.br();
        }
        //p.setFontBold(false);
        p.line(3);
        p.br(3);
        p.br();
        for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish &d = static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, 0))->fDish;
            if (d.printer == s) {
                p.setFontSize(basesize);
                p.setFontBold(true);
                p.ltext(d.name, 0);
                p.setFontSize(basesize + 8);
                p.setFontBold(true);
                p.rtext(float_str(d.qty, 2));
                p.br();
                if (d.modificator.isEmpty() == false) {
                    p.setFontSize(basesize - 4);
                    p.ltext(d.modificator, 0);
                    p.br();
                }
                p.line();
                p.br();
            }
        }
        p.br();
        p.line(3);
        p.br(3);
        p.setFontSize(basesize - 4);
       // p.setFontBold(false);

        p.ltext(QString("%1 %2").arg(tr("Printed:"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
        p.br();
        p.br();
        p.br();
        p.br();
        p.br();
        p.ltext("_", 0);
        p.br();
        QString finalPrinter = s;
        if (fPrinterAliases.contains(s)) {
            finalPrinter = fPrinterAliases[s];
        }
        p.print(finalPrinter, QPrinter::Custom);
    }
    ui->tblDishes->setEnabled(false);
    ui->wQty->setEnabled(false);
}

bool Workspace::printTax(double cardAmount, double idramAmount)
{
    C5Database db(fDBParams);
    db[":f_header"] = fOrderUuid;
    db[":f_state"] = DISH_STATE_OK;
    db.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1 "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state");
    QString useExtPos = C5Config::taxUseExtPos();
    if (idramAmount > 0.01) {
        cardAmount = idramAmount;
        useExtPos = "true";
    }
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), useExtPos, C5Config::taxCashier(), C5Config::taxPin(), this);
    while (db.nextRow()) {
        pt.addGoods(C5Config::taxDept(), db.getString("f_adgcode"), db.getString("f_dish"), db.getString("f_name"), db.getDouble("f_price"), db.getDouble("f_qty1"), 0.0);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(cardAmount, 0, jsonIn, jsonOut, err);
    db[":f_id"] = db.uuid();
    db[":f_order"] = fOrderUuid;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    if (__c5config.getValue(param_debuge_mode).toInt() == 1) {
        QSqlQuery q(db.fDb);
        pt.saveTimeResult(fOrderUuid, q);
    }
    if (result == pt_err_ok) {
        QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = fOrderUuid;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = fOrderUuid;
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
        return true;
    }
    if (C5Message::error(err, tr("Try again"), tr("Do not print")) == QDialog::Rejected) {
        return true;
    }
    return false;
}

bool Workspace::printReceipt(const QString &id, bool printSecond)
{
    C5Database db(fDBParams);
    QFont font(qApp->font());
    font.setFamily(__c5config.getValue(param_receipt_print_font_family));
    int basefont = __c5config.getValue(param_receipt_print_font_size).toInt();
    font.setPointSize(basefont);
    font.setBold(true);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    QMap<int, QMap<QString, QVariant> > packages;

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    db[":f_header"] = id;
    db.exec("select p.f_id, d.f_name, p.f_qty, p.f_price "
            "from o_package p "
            "inner join d_package d on d.f_id=p.f_package "
            "where p.f_header=:f_header");
    while (db.nextRow()) {
        QMap<QString, QVariant> p;
        for (int i = 0; i < db.columnCount(); i++) {
            p[db.columnName(i)] = db.getValue(i);
        }
        packages[db.getInt("f_id")] = p;
    }
    db[":f_id"] = id;
    db.exec("select o.f_prefix, o.f_hallid, t.f_firmname, t.f_address, t.f_dept, t.f_hvhh, t.f_devnum, "
            "t.f_serial, t.f_fiscal, t.f_receiptnumber, t.f_time as f_taxtime, concat(left(u.f_first, 1), '. ', u.f_last) as f_staff, "
            "o.f_amountcash, o.f_amountcard, o.f_amountidram, o.f_amountother, o.f_amounttotal, o.f_print, o.f_comment, ht.f_name as f_tablename, "
            "p.f_contact, p.f_phone, p.f_address "
            "from o_header o "
            "left join o_tax t on t.f_id=o.f_id "
            "left join s_user u on u.f_id=o.f_staff "
            "left join h_tables ht on ht.f_id=o.f_table "
            "left join c_partners p on p.f_id=o.f_partner "
            "where o.f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid order number"));
        return false;
    }
    p.setFontBold(true);
    p.ctext(tr("Receipt #") + QString("%1%2").arg(db.getString("f_prefix")).arg(db.getString("f_hallid")));
    p.br();
    //p.setFontBold(false);
    if (db.getString("f_receiptnumber").length() > 0) {
        p.ltext(db.getString("f_firmname"), 0);
        p.br();
        p.ltext(db.getString("f_address"), 0);
        p.br();
        p.ltext(tr("Department"), 0);
        p.rtext(db.getString("f_dept"));
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(db.getString("f_hvhh"));
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(db.getString("f_devnum"));
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(db.getString("f_serial"));
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(db.getString("f_fiscal"));
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(db.getString("f_receiptnumber"));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(db.getString("f_taxtime"));
        p.br();
        p.ltext(tr("(F)"), 0);
        p.br();
    }
    p.ltext(tr("Cashier"), 0);
    p.rtext(db.getString("f_staff"));
    p.br();
    p.ltext(tr("Delivery"), 0);
    p.rtext(db.getString("f_tablename"));
    p.br();
    p.line(3);
    p.br(2);
    p.setFontBold(true);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    //p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);

    C5Database dd(fDBParams);
    dd[":f_header"] = id;
    dd[":f_state"] = DISH_STATE_OK;
    dd.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1, b.f_package "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state "
            "order by b.f_package, b.f_row ");
    int package = 0;
    while (dd.nextRow()) {
        if (dd.getInt("f_package") > 0) {
            if (package != dd.getInt("f_package")) {
                if (package > 0) {
                    p.br();
                    p.line();
                    p.br(2);
                    package = 0;
                }
                p.setFontBold(true);
                p.setFontSize(basefont);
                package = dd.getInt("f_package");
                p.ltext(packages[package]["f_name"].toString(), 0);
                p.br();
                p.ltext(QString("%1 x %2 = %3")
                        .arg(float_str(packages[package]["f_qty"].toDouble(), 2))
                        .arg(packages[package]["f_price"].toDouble(), 2)
                        .arg(packages[package]["f_qty"].toDouble() * packages[package]["f_price"].toDouble()), 0);
                p.br();
                //p.setFontBold(false);
                p.setFontSize(basefont);
            }
            p.ltext(QString("*** %1 %2, %3 x%4").arg(tr("Class:")).arg(dd.getString("f_adgcode")).arg(dd.getString("f_name")).arg(float_str(dd.getDouble("f_qty1"), 2)), 0);
            p.br();
        } else {
            if (package > 0) {
                p.br();
                p.line();
                p.br(2);
                package = 0;
            }
            if (dd.getString(0).isEmpty()) {
                p.ltext(QString("%3").arg(dd.getString("f_name")), 0);
            } else {
                p.ltext(QString("%1 %2, %3").arg(tr("Class:")).arg(dd.getString("f_adgcode")).arg(dd.getString("f_name")), 0);
            }
            p.br();
            p.ltext(QString("%1 x %2 = %3").arg(float_str(dd.getDouble("f_qty1"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_qty1") * dd.getDouble("f_price"), 2)), 0);
            p.br();
            p.line();
            p.br(2);
        }
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(db.getDouble("f_amounttotal"), 2));
    p.br();
    p.br();

    p.line();
    p.br();

    if (db.getDouble("f_amountcash") > 0.001) {
        p.ltext(tr("Payment, cash"), 0);
        p.rtext(float_str(db.getDouble("f_amountcash"), 2));
    }
    if (db.getDouble("f_amountcard") > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(db.getDouble("f_amountcard"), 2));
    }
    if (db.getDouble("f_amountidram") > 0.001) {
        p.ltext(tr("Payment, Idram"), 0);
        p.rtext(float_str(db.getDouble("f_amountidram"), 2));
    }
    if (db.getDouble("f_amountother") > 0.001) {
        p.ltext(tr("Payment, other"), 0);
        p.rtext(float_str(db.getDouble("f_amountother"), 2));
    }

    if (db.getString("f_phone").length() > 0) {
        p.br();
        p.br();
        p.ltext(tr("Customer"), 0);
        p.br();
        p.ltext(db.getString("f_phone"), 0);
        p.br();
        p.ltext(db.getString("f_address"), 0);
        p.br();
        p.ltext(db.getString("f_contact"), 0);
        p.br();
    }

    p.setFontSize(basefont - 2);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(__c5config.getValue(param_recipe_footer_text), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(db.getInt("f_print")), 0);
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.ltext(".", 0);
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);

    db[":f_print"] = db.getInt("f_print") + 1;
    db.update("o_header", "f_id", id);

    if (printSecond && __c5config.getValue(param_shop_print_v2) == "1") {
        printReceipt(id, false);
    }
    return true;
}

void Workspace::on_btnAppMenu_clicked()
{
    MenuDialog m;
    m.exec();
}

void Workspace::on_btnP05_2_clicked()
{
    setQty(0.25, 1);
}

void Workspace::on_btnP3_clicked()
{
    setQty(3, 1);
}

void Workspace::on_btnP4_clicked()
{
    setQty(4, 1);
}

void Workspace::on_btnP5_clicked()
{
    setQty(5, 1);
}

void Workspace::on_btnP10_clicked()
{
    setQty(10, 2);
}

void Workspace::on_btnReprintLastCheck_clicked()
{
    if (fPreviouseUuid.isEmpty()) {
        return;
    }
    printReceipt(fPreviouseUuid, false);
}

void Workspace::on_leReadCode_textChanged(const QString &arg1)
{
    if (arg1 == "+") {
        ui->leReadCode->clear();
        setQty(1, 2);
        return;
    }
    if (arg1 == "-") {
        ui->leReadCode->clear();
        setQty(1, 3);
    }
}

void Workspace::addDishToOrder(Dish *d)
{
    int row = 0;
    bool isnew = true;
    if (__c5config.getValue(param_zip_dish_in_order).toInt() == 1) {
        for (int i = 0; i < ui->tblOrder->rowCount(); i++) {
            OrderDish *od = static_cast<OrderDish*>(ui->tblOrder->cellWidget(i, 0));
            if (d->id == od->fDish.id) {
                row = i;
                od->fDish.qty++;
                od->updateInfo();
                isnew = false;
                break;
            }
        }
    }
    if (isnew) {
        row = ui->tblOrder->rowCount();
        ui->tblOrder->setRowCount(row + 1);
        OrderDish *od = new OrderDish(*d);
        ui->tblOrder->setCellWidget(row, 0, od);
    }
    on_tblOrder_cellClicked(row, 0);
    countTotal();
}

void Workspace::on_btnComment_clicked()
{
    int row = ui->tblOrder->currentRow();
    if (row < 0) {
        return;
    }
    OrderDish *od = static_cast<OrderDish*>(ui->tblOrder->cellWidget(row, 0));
    Dish &d = od->fDish;
    QString comment;
    if (DlgListOfDishComments::getComment(d.name, comment)) {
        d.modificator = comment;
        od->updateInfo();
    }
}

