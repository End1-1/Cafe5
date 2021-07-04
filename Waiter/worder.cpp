#include "worder.h"
#include "ui_worder.h"
#include "c5ordertabledelegate.h"
#include "c5orderdriver.h"
#include "c5message.h"
#include "c5utils.h"
#include "dishitem.h"
#include "dlgorder.h"
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

void WOrder::addStretch()
{
    ui->vl->addStretch();
}

void WOrder::itemsToTable()
{
    while (ui->vl->itemAt(0)) {
        ui->vl->itemAt(0)->widget()->deleteLater();
        ui->vl->removeItem(ui->vl->itemAt(0));
    }
    if (fOrderDriver->currentOrderId().isEmpty()) {
        return;
    }
    for (int i = 0; i < fOrderDriver->dishesCount(); i++) {
        DishItem *w = new DishItem(fOrderDriver, i);
        connect(w, SIGNAL(focused(int)), this, SLOT(focused(int)));
        if (fDlg) {
            connect(w, &DishItem::changeQty, fDlg, &DlgOrder::changeQty);
        } else {
            w->setReadyOnly(true);
        }
        ui->vl->addWidget(w);
    }
}

int WOrder::addItem(QJsonObject o)
{
    o["f_row"] = fOrderDriver->dishesCount();
    if (!fOrderDriver->addDish(o)) {
        C5Message::error(fOrderDriver->error());
        return -1;
    }
    int row = fOrderDriver->dishesCount() - 1;
    DishItem *di = new DishItem(fOrderDriver, row);
    connect(di, &DishItem::focused, this, &WOrder::focused);
    if (fDlg) {
        connect(di, &DishItem::changeQty, fDlg, &DlgOrder::changeQty);
    } else {
        di->setReadyOnly(true);
    }
    ui->vl->insertWidget(row, di);
    focused(row);
    setMinimumHeight(((row + 1) *50) + 10);
    return row;
}

QPoint WOrder::updateItem(int index)
{
    QPoint p = focused(index);
    return p;
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
