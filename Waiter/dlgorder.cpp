#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "c5cafecommon.h"
#include "c5user.h"
#include "c5witerconf.h"
#include "c5menu.h"
#include "c5order.h"
#include "dlgdishremovereason.h"
#include "c5dishtabledelegate.h"
#include "c5ordertabledelegate.h"
#include "c5part2tabledelegate.h"
#include "dlgpayment.h"
#include "dlgpassword.h"
#include "dlgguest.h"
#include "dlglistofdishcomments.h"
#include "dlgface.h"
#include "dlglistofmenu.h"
#include <QCloseEvent>
#include <QScrollBar>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

DlgOrder::DlgOrder(QWidget *parent) :
    C5Dialog(C5Config::dbParams(), parent),
    ui(new Ui::DlgOrder)
{
    ui->setupUi(this);
    fOrder = new C5Order();
    ui->btnGuest->setVisible(C5Config::useHotel());
}

DlgOrder::~DlgOrder()
{
    delete ui;
    delete fOrder;
}

void DlgOrder::openTable(const QJsonObject &table, C5User *user)
{
    QJsonObject hall = C5CafeCommon::hall(table["f_hall"].toString());
    if (hall.count() > 0) {
        C5Config::setServiceFactor(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_service_factor]);
        QString menuName = C5Menu::fMenuNames[C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_default_menu]];
        C5Config::setValue(param_default_menu_name, menuName);
    }
    DlgOrder *d = new DlgOrder(C5Config::fParentWidget);
    d->fMenuName = C5Config::defaultMenuName();
    d->showFullScreen();
    d->hide();
    qApp->processEvents();
    d->ui->tblPart1->fitRowToHeight();
    d->ui->tblPart1->fitColumnsToWidth();
    d->ui->tblPart2->setItemDelegate(new C5Part2TableDelegate());
    d->ui->tblPart2->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    d->ui->tblPart2->fitColumnsToWidth();
    d->ui->tblPart2->verticalHeader()->setDefaultSectionSize(PART2_ROW_HEIGHT);
    d->ui->tblPart2->fitRowToHeight();
    d->ui->tblDishes->setItemDelegate(new C5DishTableDelegate());
    d->ui->tblDishes->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    d->ui->tblDishes->fitColumnsToWidth();
    d->ui->tblDishes->verticalHeader()->setDefaultSectionSize(PART3_ROW_HEIGHT);
    d->ui->tblDishes->fitRowToHeight();
    d->ui->tblOrder->setItemDelegate(new C5OrderTableDelegate());
    d->ui->tblOrder->setColumnWidth(0, d->ui->tblOrder->width() - 2);
    d->ui->tblOrder->verticalHeader()->setDefaultSectionSize(PART4_ROW_HEIGHT);
    d->buildMenu(d->fMenuName, "", "");
    d->fUser = user;
    d->ui->lbTable->setText(table["f_name"].toString());
    d->ui->lbStaffName->setText(user->fFull);
    d->load(table["f_id"].toString().toInt());
    d->exec();
    delete d;
}

void DlgOrder::accept()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    fOrder->setHeaderValue("unlocktable", "1");
    fOrder->save(sh);
}

void DlgOrder::reject()
{
    accept();
}

void DlgOrder::load(int table)
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleOpenTable(QJsonObject)));
    sh->bind("cmd", sm_opentable);
    sh->bind("table", table);
    sh->bind("host", hostinfo);
    sh->send();
}

void DlgOrder::buildMenu(const QString &menu, QString part1, QString part2)
{
    if (menu.isEmpty()) {
        C5Message::error(tr("Menu is not defined"));
        return;
    }
    QStringList p1 = C5Menu::fMenu[menu].keys();
    int col = 0, row = 0;
    ui->tblPart1->clear();
    foreach (QString s, p1) {
        ui->tblPart1->setItem(row, col++, new QTableWidgetItem(s));
        if (col == 2) {
            col = 0;
            row ++;
        }
    }

    if (part1.isEmpty()) {
        if (!ui->tblPart1->item(0, 0)) {
            C5Message::error(tr("Menu is not defined"));
            return;
        }
        part1 = ui->tblPart1->item(0, 0)->text();
    }

    ui->tblPart2->clear();
    ui->tblPart2->setRowCount(0);
    QStringList p2 = C5Menu::fMenu[menu][part1].keys();
    int colCount = ui->tblPart2->columnCount();
    col = 0;
    row = 0;
    foreach (QString s, p2) {
        if (row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem(s);
        item->setData(Qt::UserRole, C5Menu::fPart2Color[s]);
        ui->tblPart2->setItem(row, col++, item);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
    if (ui->tblPart2->rowCount() > 0) {
        for (int i = 0; i < ui->tblPart2->columnCount(); i++) {
            QTableWidgetItem *item = ui->tblPart2->item(ui->tblPart2->rowCount() - 1, i);
            if (!item) {
                item = new QTableWidgetItem("");
                item->setData(Qt::UserRole, -1);
                ui->tblPart2->setItem(ui->tblPart2->rowCount() - 1, i, item);
            }
        }
    }

    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);
    if (part2.isEmpty()) {
        return;
    }
    colCount = ui->tblDishes->columnCount();
    col = 0;
    row = 0;
    QList<QJsonObject> dishes = C5Menu::fMenu[menu][part1][part2];
    for (int i = 0; i < dishes.count(); i++) {
        QJsonObject &o = dishes[i];
        QTableWidgetItem *item = new QTableWidgetItem(o["f_name"].toString());
        item->setData(Qt::UserRole, o);
        if (row > ui->tblDishes->rowCount() - 1) {
            ui->tblDishes->setRowCount(row + 1);
        }
        ui->tblDishes->setItem(row, col++, item);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
}

void DlgOrder::addDishToOrder(const QJsonObject &obj)
{
    QJsonObject o = obj;
    if (obj["f_id"].toString().toInt() == 0) {
        o["f_id"] = "";
        o["f_header"] = fOrder->headerValue("f_id");
        o["f_state"] = QString::number(DISH_STATE_OK);
        o["f_service"] = C5Config::serviceFactor();
        o["f_discount"] = fOrder->headerValue("f_discountfactor");
        o["f_total"] = "0";
        o["f_qty1"] = "1";
        o["f_qty2"] = "0";
        o["f_comment"] = "";
    }
    int row = ui->tblOrder->rowCount();
    ui->tblOrder->setRowCount(row + 1);
    ui->tblOrder->setItem(row, 0, new QTableWidgetItem());
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->setCurrentCell(ui->tblOrder->rowCount() - 1, 0);
    fOrder->addItem(o);
    fOrder->setHeaderValue("f_amountcash", 0);
    fOrder->setHeaderValue("f_amountcard", 0);
    fOrder->setHeaderValue("f_amountbank", 0);
    fOrder->setHeaderValue("f_amountother", 0);
    fOrder->countTotal();
}

void DlgOrder::loadOrder(const QJsonObject &obj)
{
    fOrder->fHeader = obj["header"].toObject();
    if (fOrder->headerValue("f_id").toInt() == 0) {
        fOrder->setHeaderValue("f_staff", fUser->fId);
        fOrder->setHeaderValue("f_staffname", fUser->fFull);
        fOrder->setHeaderValue("f_state", ORDER_STATE_OPEN);
        fOrder->setHeaderValue("f_comment", "");
        fOrder->setHeaderValue("f_hall", obj["table"].toArray().at(0)["f_hall"].toString());
        fOrder->setHeaderValue("f_amounttotal", 0);
        fOrder->setHeaderValue("f_amountcash", 0);
        fOrder->setHeaderValue("f_amountcard", 0);
        fOrder->setHeaderValue("f_amountbank", 0);
        fOrder->setHeaderValue("f_amountother", 0);
        fOrder->setHeaderValue("f_amountservice", 0);
        fOrder->setHeaderValue("f_amountdiscount", 0);
        fOrder->setHeaderValue("f_servicefactor", C5Config::serviceFactor());
        fOrder->setHeaderValue("f_discountfactor", 0);
    }
    fOrder->setHeaderValue("f_currentstaff", fUser->fId);
    fOrder->setHeaderValue("f_currentstaffname", fUser->fFull);
    fOrder->fItems = obj["body"].toArray();
    itemsToTable();
    if (fOrder->headerValue("f_discountfactor").toDouble() > 0) {
        ui->lbDiscount->setEnabled(true);
        ui->lbDiscount->setText(QString("[%1: %2%]").arg(tr("Discount")).arg(fOrder->headerValue("f_discountfactor").toDouble() * 100));
    } else {
        ui->lbDiscount->setEnabled(false);
        ui->lbDiscount->setText(tr("[Discount]"));
    }
    if (fOrder->headerValue("f_servicefactor").toDouble() > 0) {
        ui->lbService->setEnabled(true);
        ui->lbService->setText(QString("[%1: %2%]").arg(tr("Service")).arg(fOrder->headerValue("f_servicefactor").toDouble() * 100));
    } else {
        ui->lbService->setEnabled(false);
        ui->lbService->setText(tr("[Service]"));
    }
    if (fOrder->headerValue("f_staff").toInt() != fOrder->headerValue("f_currentstaff").toInt()) {
        C5Message::info(QString("%1\r\n%2").arg(tr("Order owner")).arg(fOrder->headerValue("f_staffname")));
    }
}

void DlgOrder::changeQty(double qty)
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int index = ml.at(0).row();
    QJsonObject o = fOrder->fItems[index].toObject();
    if (qty > 0) {
        if (o["f_qty1"].toString().toDouble() + qty > 99) {
            return;
        }
        o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() + qty, 'f', 2);
    } else {
        if (o["f_qty1"].toString().toDouble() + qty < 0.4) {
            return;
        }
        if (o["f_qty2"].toString().toDouble() < o["f_qty1"].toString().toDouble() + qty) {
            o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() + qty, 'f', 1);
        }
    }
    fOrder->fItems[index] = o;
    fOrder->countTotal();
    ui->tblOrder->item(index, 0)->setData(Qt::UserRole, fOrder->fItems[index].toObject());
    ui->tblOrder->viewport()->update();
}

void DlgOrder::itemsToTable()
{
    fOrder->countTotal();
    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(fOrder->fItems.count());
    for (int i = 0; i < fOrder->fItems.count(); i++) {
        ui->tblOrder->setItem(i, 0, new QTableWidgetItem());
        ui->tblOrder->item(i, 0)->setData(Qt::UserRole, fOrder->fItems[i].toObject());
        if (fOrder->itemValue(i, "f_state").toInt() != DISH_STATE_OK) {
            ui->tblOrder->setRowHidden(i, true);
        }
    }
    ui->leTotal->setText(fOrder->headerValue("f_amounttotal"));
}

void DlgOrder::saveOrder()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    sh->bind("cmd", sm_saveorder);
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgOrder::handleOpenTable(const QJsonObject &obj)
{
    sender()->deleteLater();
    switch (obj["reply"].toInt()) {
    case 0:
    case 2: {
        C5Message::error(obj["msg"].toString());
        QDialog::accept();
        return;
    }
    case 3: {
        C5SocketHandler *sh = createSocketHandler(SLOT(handleOpenTable(QJsonObject)));
        sh->bind("cmd", sm_opentable);
        sh->bind("table", obj["table"].toInt());
        sh->bind("host", hostinfo);
        sh->send();
        return;
    }
    case 1: {
        loadOrder(obj);
        break;
    }
    }
}

void DlgOrder::handlePrintService(const QJsonObject &obj)
{
    fOrder->fItems = obj["body"].toArray();
    fOrder->fHeader = obj["header"].toObject();
    itemsToTable();
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    fOrder->save(sh);
}

void DlgOrder::saveAndQuit(const QJsonObject &obj)
{
    if (obj["header"].toObject()["unlocktable"].toString().toInt() > 0) {
        QDialog::accept();
        return;
    }
    fOrder->fItems = obj["body"].toArray();
    fOrder->fHeader = obj["header"].toObject();
    itemsToTable();
}

void DlgOrder::handleError(int err, const QString &msg)
{
    switch (err) {
    default:
        break;
    }
    C5Message::error(msg);
}

void DlgOrder::on_tblPart1_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    fPart1Name = item->text();
    buildMenu(fMenuName, fPart1Name, "");
}

void DlgOrder::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }

    QString part2 = item->text();
    if (part2.isEmpty()) {
        return;
    }
    buildMenu(fMenuName, fPart1Name, part2);
}

void DlgOrder::on_tblDishes_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    const QJsonObject &o = item->data(Qt::UserRole).toJsonObject();
    addDishToOrder(o);
}

void DlgOrder::on_btnPlus1_clicked()
{
    changeQty(1);
}

void DlgOrder::on_btnMin1_clicked()
{
    changeQty(-1);
}

void DlgOrder::on_btnPlus05_clicked()
{
    changeQty(0.5);
}

void DlgOrder::on_btnMin05_clicked()
{
    changeQty(-0.5);
}

void DlgOrder::on_btnCustom_clicked()
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int index = ml.at(0).row();
    QJsonObject o = fOrder->fItems.at(index).toObject();
    int qty;
    if (!DlgPassword::getQty(o["f_qty1"].toString(), qty)) {
        return;
    }
    if (qty > o["f_qty2"].toString().toDouble()) {
        o["f_qty1"] = QString::number(qty);
    }
    fOrder->fItems[index] = o;
    ui->tblOrder->item(index, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->viewport()->update();
}

void DlgOrder::on_btnPrintService_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintService(QJsonObject)));
    sh->bind("cmd", sm_printservice);
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgOrder::on_btnPayment_clicked()
{
    bool empty = true;
    for (int i = 0; i < fOrder->fItems.count(); i++) {
        if (fOrder->itemValue(i, "f_state").toInt() != DISH_STATE_OK) {
            continue;
        }
        if (fOrder->itemValue(i, "f_qty1").toDouble() > fOrder->itemValue(i, "f_qty2").toDouble()) {
            C5Message::error(tr("Order is incomplete"));
            return;
        }
        empty = false;
    }
    if (empty) {
        C5Message::error(tr("Order is incomplete"));
        return;
    }
    int paymentResult = DlgPayment::payment(fOrder);
    switch (paymentResult) {
    case PAYDLG_ORDER_CLOSE:
        QDialog::accept();
        break;
    case PAYDLG_NONE:
        saveOrder();
        break;
    }
}

void DlgOrder::on_btnExit_clicked()
{
    accept();
}

void DlgOrder::on_btnVoid_clicked()
{
    int index = ui->tblOrder->currentRow();
    if (index < 0) {
        return;
    }
    QJsonObject o = fOrder->fItems.at(index).toObject();
    if (o["f_qty2"].toString().toDouble() < 0.01) {
        if (C5Message::question(QString("%1<br>%2(%3)").arg(tr("Confirm to remove")).arg(o["f_name"].toString()).arg(o["f_qty1"].toString())) != QDialog::Accepted) {
            return;
        }
        o["f_state"] = DISH_STATE_NONE;
        fOrder->fItems[index] = o;
        saveOrder();
        return;
    } else {
        double max = 0;
        if (fUser->check(cp_t5_remove_printed_service)) {
            max = o["f_qty1"].toString().toDouble();
        } else {
            max = o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble();
        }
        if (max < 0.01) {
            return;
        }
        if (!DlgPassword::getAmount(o["f_name"].toString(), max, true)) {
            return;
        }
        if (max <= o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble()) {
            o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() - max);
        } else {
            QString reasonname;
            int reasonid;
            if (!DlgDishRemoveReason::getReason(reasonname, reasonid)) {
                return;
            }
            if (max > o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble()) {
                max -= o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble();
            }
            //GET DISH STATE
            int dishState = reasonid;
            if (max < o["f_qty1"].toString().toDouble()) {
                o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() - max);
                QJsonObject o2 = o;
                o2["f_id"] = "0";
                o2["f_state"] = QString::number(dishState * -1); // Print removed dishes if state < 0
                o2["f_qty1"] = QString::number(max, 'f', 2);
                o2["f_qty2"] = o2["f_qty1"];
                o2["f_removereason"] = reasonname;
                fOrder->fItems.append(o2);
                if (o2["f_qty2"].toString().toDouble() > o2["f_qty1"].toString().toDouble()) {
                    o2["f_qty2"] = o2["f_qty1"];
                }
            } else {
                o["f_state"] = QString::number(dishState * -1); // Print removed dishes if state < 0
                o["f_removereason"] = reasonname;
            }
        }
        if (o["f_qty2"].toString().toDouble() > o["f_qty1"].toString().toDouble()) {
            o["f_qty2"] = o["f_qty1"];
        }
        fOrder->fItems[index] = o;
        if (fOrder->headerValue("f_print").toInt() > 0) {
            fOrder->setHeaderValue("f_print", fOrder->headerValue("f_print").toInt() * -1);
        }
        saveOrder();
    }
}

void DlgOrder::on_btnComment_clicked()
{
    int index = ui->tblOrder->currentRow();
    if (index < 0) {
        return;
    }
    QJsonObject o = fOrder->fItems.at(index).toObject();
    QString comment = o["f_name"].toString();
    if (!DlgListOfDishComments::getComment(comment)) {
        return;
    }
    o["f_comment"] = comment;
    fOrder->fItems[index] = o;
    ui->tblOrder->item(index, 0)->setData(Qt::UserRole, fOrder->fItems[index].toObject());
    ui->tblOrder->viewport()->update();
}

void DlgOrder::on_btnDishScrollDown_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() + 3);
}

void DlgOrder::on_btnTypeScrollDown_clicked()
{
    ui->tblPart2->verticalScrollBar()->setValue(ui->tblPart2->verticalScrollBar()->value() + 2);
}

void DlgOrder::on_btnTypeScrollUp_clicked()
{
    ui->tblPart2->verticalScrollBar()->setValue(ui->tblPart2->verticalScrollBar()->value() - 2);
}

void DlgOrder::on_btnDishScrollUp_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() - 3);
}

void DlgOrder::on_btnExpandDishTable_clicked()
{
    if (ui->wDishTypes->isVisible()) {
        ui->wDishTypes->setVisible(false);
        ui->wDishTypesControl->setVisible(false);
        ui->btnExpandDishTable->setIcon(QIcon(":/minimize.png"));
    } else {
        ui->wDishTypes->setVisible(true);
        ui->wDishTypesControl->setVisible(true);
        ui->btnExpandDishTable->setIcon(QIcon(":/expand2.png"));
    }
}

void DlgOrder::on_btnChangeMenu_clicked()
{
    QString menu;
    if (!DlgListOfMenu::getMenuId(menu, fDBParams)) {
        return;
    }
    fMenuName = menu;
    buildMenu(menu, "", "");
}

void DlgOrder::on_btnChangeTable_clicked()
{
    int tableId;
    if (!DlgFace::getTable(tableId)) {
        return;
    }
    QJsonObject table = C5CafeCommon::table(tableId);
    if (table.count() > 0) {
        QJsonObject hall = C5CafeCommon::hall(table["f_hall"].toString());
        if (hall.count() > 0) {
            C5Config::setServiceFactor(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_service_factor]);
            QString menuName = C5Menu::fMenuNames[C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_default_menu]];
            if (fMenuName != menuName) {
                fMenuName = menuName;
                buildMenu(fMenuName, "", "");
            }
        }
    }
    load(tableId);
}

void DlgOrder::on_btnGuest_clicked()
{
    QString res, inv, room, guest;
    DlgGuest::getGuest(res, inv, room, guest);
}
