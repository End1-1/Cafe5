#include "dlgsplitorder.h"
#include "ui_dlgsplitorder.h"
#include "c5orderdriver.h"
#include "dlgface.h"
#include "dlgpassword.h"
#include "datadriver.h"
#include <QScrollBar>

DlgSplitOrder::DlgSplitOrder(C5User *user) :
    C5Dialog(__c5config.dbParams()),
    fUser(user),
    ui(new Ui::DlgSplitOrder)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
}

DlgSplitOrder::~DlgSplitOrder()
{
    delete ui;
}

void DlgSplitOrder::configOrder(const QString &orderId)
{
    if (!ui->o1->fOrderDriver->loadData(orderId)) {
        C5Message::error(ui->o1->fOrderDriver->error());
        return;
    }
    ui->o1->itemsToTable();
    ui->lbLeft->setText(dbtable->name(ui->o1->fOrderDriver->headerValue("f_table").toInt()));
    ui->lbRight->setText(tr("Select table"));
}

void DlgSplitOrder::on_btnChoseTable_clicked()
{
    if (!ui->o2->fOrderDriver->currentOrderId().isEmpty()) {
        if (C5Message::question(tr("Save orders?")) == QDialog::Accepted) {
            if (!ui->o1->fOrderDriver->save()) {
                C5Message::error(ui->o1->fOrderDriver->error());
            }
            if (!ui->o2->fOrderDriver->save()) {
                C5Message::error(ui->o2->fOrderDriver->error());
            }
        } else {
            ui->o1->fOrderDriver->reloadOrder();
            ui->o1->itemsToTable();
        }
    }
    QString err;
    QStringList orders;
    int tableId;
    if (!DlgFace::getTable(tableId, ui->o1->fOrderDriver->headerValue("f_hall").toInt(), fUser)) {
        return;
    }
    if (tableId == ui->o1->fOrderDriver->headerValue("f_table").toInt()) {
        C5Message::error(tr("Same table"));
        return;
    }
    if (!dbtable->closeTable(tableId, err)) {
        C5Message::error(ui->o2->fOrderDriver->error());
        return;
    }
    if (!dbtable->openTable(tableId, orders, err)) {
        C5Message::error(err);
        reject();
        return;
    }
    if (orders.count() == 0) {
        if (C5Message::question(tr("Open new order")) != QDialog::Accepted) {
            return;
        }
        QString id;
        ui->o2->fOrderDriver->newOrder(fUser->id(), id, tableId);
    } else {
        ui->o2->fOrderDriver->setCurrentOrderID(orders.at(0));
    }
    ui->o2->setDlg(nullptr);
    ui->o2->itemsToTable();
    ui->lbRight->setText(dbtable->name(ui->o2->fOrderDriver->headerValue("f_table").toInt()));
}

void DlgSplitOrder::on_btnExit_clicked()
{
    accept();
}

void DlgSplitOrder::on_btnLeftToRight_clicked()
{
    int row;
    if (!ui->o1->currentRow(row)) {
        return;
    }
    moveItem(ui->o1->fOrderDriver, ui->o2->fOrderDriver, row);
}

bool DlgSplitOrder::moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row)
{
    if (or2->currentOrderId().isEmpty()) {
        C5Message::error(tr("Right side table not selected"));
        return false;
    }
    if (or1->dishesValue("f_qty1", row).toDouble() > 1) {
        double max = or1->dishesValue("f_qty1", row).toDouble();
        if (!DlgPassword::getAmount(dbdish->name(or1->dishesValue("f_dish", row).toInt()), max, true)) {
            return false;
        }
        if (max < or1->dishesValue("f_qty1", row).toDouble()) {
            int duprow = or1->duplicateDish(row);
            or1->setDishesValue("f_qty1", max, duprow);
            if (or1->dishesValue("f_qty2", duprow).toDouble() > or1->dishesValue("f_qty1", duprow).toDouble()) {
                or1->setDishesValue("f_qty2", or1->dishesValue("f_qty1", duprow), duprow);
            }
            or1->setDishesValue("f_qty1", or1->dishesValue("f_qty1", row).toDouble() - max, row);
            if (or1->dishesValue("f_qty2", row).toDouble() > or1->dishesValue("f_qty1", row).toDouble()) {
                or1->setDishesValue("f_qty2", or1->dishesValue("f_qty1", row), row);
            }
            row = duprow;
        }
    }
    or1->setDishesValue("f_fromtable", or1->headerValue("f_table"), row);
    if (!or2->addDish(or1->dish(row))) {
        C5Message::error(or2->error());
        return false;
    }
    or1->removeDish(row);
    ui->o1->itemsToTable();
    ui->o2->itemsToTable();
    return true;
}

void DlgSplitOrder::on_btnAllLeftToRigh_clicked()
{
    if (C5Message::question(tr("Confirm to move whole order from left to right")) != QDialog::Accepted) {
        return;
    }
    while (ui->o1->fOrderDriver->dishesCount() > 0) {
        if (!moveItem(ui->o1->fOrderDriver, ui->o2->fOrderDriver, 0)) {
            return;
        }
    }
}

void DlgSplitOrder::on_btnRightToLeft_clicked()
{
    int row;
    if (!ui->o2->currentRow(row)) {
        return;
    }
    moveItem(ui->o2->fOrderDriver, ui->o1->fOrderDriver, row);
}

void DlgSplitOrder::on_btnAllRightToLeft_clicked()
{
    if (C5Message::question(tr("Confirm to move whole order from right to left")) != QDialog::Accepted) {
        return;
    }
    while (ui->o1->fOrderDriver->dishesCount() > 0) {
        if (!moveItem(ui->o2->fOrderDriver, ui->o1->fOrderDriver, 0)) {
            return;
        }
    }
}

void DlgSplitOrder::on_btnSave_clicked()
{
    if (!ui->o1->fOrderDriver->save()) {
        C5Message::error(ui->o1->fOrderDriver->error());
    }
    if (!ui->o2->fOrderDriver->currentOrderId().isEmpty()) {
        if (!ui->o2->fOrderDriver->save()) {
            C5Message::error(ui->o2->fOrderDriver->error());
        }
    }
    accept();
}

void DlgSplitOrder::on_btnScrollUp1_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() + 300);
}

void DlgSplitOrder::on_btnScrollDown1_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() - 300);
}

void DlgSplitOrder::on_btnScrollUp2_clicked()
{
    ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->value() + 300);
}

void DlgSplitOrder::on_btnScrollDown2_clicked()
{
    ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->value() - 300);
}