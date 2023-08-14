#include "worderw.h"
#include "ui_worder.h"
#include "c5ordertabledelegate.h"
#include "c5orderdriver.h"
#include "c5message.h"
#include "c5utils.h"
#include "dishitem.h"
#include "dlgorder.h"
#include "dlgguestinfo.h"
#include "dlgqty.h"
#include <QJsonObject>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 50

WOrder::WOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    fSelected = false;
    fOrderDriver = new C5OrderDriver(__c5config.dbParams());
    fDlg = nullptr;
    setChanges();
}

WOrder::~WOrder()
{
    delete ui;
    delete fOrderDriver;
}

void WOrder::setDlg(DlgOrder *dlg)
{
    fDlg = dlg;
}

void WOrder::updateDishes()
{
    int r = -1;
    for (int i = 0; i < fItems.count(); i++) {
        if (fItems.at(i)->isFocused()) {
            r = i;
        }
        fItems.at(i)->deleteLater();
    }
    fItems.clear();
    while (ui->vl->itemAt(0)) {
        ui->vl->removeItem(ui->vl->itemAt(0));
    }
    if (fOrderDriver->currentOrderId().isEmpty()) {
        return;
    }
    for (int i = 0; i < fOrderDriver->dishesCount(); i++) {
        DishItem *item = new DishItem(fOrderDriver, i);
        connect(item, &DishItem::focused, this, &WOrder::focused);
        ui->vl->addWidget(item);
        fItems.append(item);
        item->clearFocus(r);
    }
    ui->vl->addStretch(1);
}

void WOrder::checkAllItems(bool v)
{
    for (auto *w: fItems) {
        w->setChecked(v);
    }
}

QList<int> WOrder::checkedItems()
{
    QList<int> result;
    for (int i = 0; i < fItems.count(); i++) {
        if (fOrderDriver->dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
            if (fItems.at(i)->isChecked()) {
                result.append(i);
            }
        }
    }
    return result;
}

int WOrder::addItem(int menuid, const QString &comment, double price)
{
    if (!fOrderDriver->addDish(menuid, comment, price)) {
        C5Message::error(fOrderDriver->error());
        return -1;
    }
    int row = fOrderDriver->dishesCount() - 1;
    DishItem *di = new DishItem(fOrderDriver, row);
    fItems.append(di);
    connect(di, &DishItem::focused, this, &WOrder::focused);
    if (fDlg) {

    } else {
        di->setReadyOnly(true);
    }
    ui->vl->insertWidget(row, di);
    focused(row);
    setMinimumHeight(((row + 5) * 50) + 10);
    return row;
}

void WOrder::updateItem(int index)
{
    for (auto *item: fItems) {
        item->setChanges();
    }
}

bool WOrder::currentRow(int &row)
{
    row = -1;
    for (int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());
        if (d) {
            if (d->isFocused()) {
                row = d->index();
                break;
            }
        }
    }
    return row > -1;
}

void WOrder::setCurrentRow(int row)
{
    for (int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());
        if (d) {
            if (i == row) {
                d->clearFocus(row);
            } else {
                d->clearFocus(-1);
            }
        }
    }
}

void WOrder::setSelected(bool v)
{
    fSelected = v;
    setChanges();
    if (!fSelected) {
        for (int i = 0, count = ui->vl->count(); i < count; i++) {
            QLayoutItem *l = ui->vl->itemAt(i);
            DishItem *d = dynamic_cast<DishItem*>(l->widget());
            if (d) {
                d->clearFocus(-1);
            }
        }
    }
}

bool WOrder::isSelected()
{
    return fSelected;
}

void WOrder::setCheckMode(bool v)
{
    for (int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());
        if (d) {
            d->setCheckMode(v);
        }
    }
}

DishItem *WOrder::dishWidget(int i)
{
    return fItems[i];
}

void WOrder::setChanges()
{
    ui->frameHeader->setProperty("guest_selected", fSelected ? "1" : "2");
    ui->frameHeader->style()->polish(ui->frameHeader);
}

QPoint WOrder::focused(int index)
{
    if (!fSelected) {
        emit activated();
    }
    QPoint p(0, 0);
    for (int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());
        if (d) {
            d->clearFocus(index);
            if (d->isFocused()) {
                p = d->mapTo(fDlg, QPoint(0, 0));
            }
        }
    }
    return p;
}

void WOrder::on_btnActivate_clicked()
{
    emit activated();
}

void WOrder::on_btnEditGuestName_clicked()
{
    DlgGuestInfo gi;
    gi.exec();
    if (gi.fCode > 0) {

    }
}
